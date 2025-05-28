// async_logger_system.cpp
// 高性能异步日志系统（含 lock-free 队列、日志级别、ring buffer、线程 ID）

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/syscall.h>
#endif

// 日志级别定义
enum class LogLevel { INFO, WARN, ERROR };

const char* levelToString(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

// 获取当前时间戳字符串
std::string current_time_str() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%F %T") << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return ss.str();
}

// 获取线程 ID
uint64_t get_thread_id() {
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return syscall(SYS_gettid);
#endif
}

// 单个日志条目
struct LogEntry {
    LogLevel level;
    std::string func;
    std::string module;
    int status;
    long long duration_ms;
    std::string timestamp;
    uint64_t thread_id;
};

// Lock-free Ring Buffer
constexpr size_t LOG_BUFFER_SIZE = 1024;
class RingLogBuffer {
public:
    bool push(const LogEntry& entry) {
        size_t next = (_head + 1) % LOG_BUFFER_SIZE;
        if (next == _tail.load(std::memory_order_acquire)) return false; // full
        _buffer[_head] = entry;
        _head = next;
        return true;
    }

    std::vector<LogEntry> flush() {
        std::vector<LogEntry> batch;
        while (_tail != _head) {
            batch.push_back(_buffer[_tail]);
            _tail = (_tail + 1) % LOG_BUFFER_SIZE;
        }
        return batch;
    }

private:
    LogEntry _buffer[LOG_BUFFER_SIZE];
    size_t _head = 0;
    std::atomic<size_t> _tail{0};
};

// 日志后台线程
class AsyncLogger {
public:
    AsyncLogger(LogLevel min_level = LogLevel::INFO) : _min_level(min_level), _running(true) {
        _thread = std::thread([this]() { this->run(); });
    }

    ~AsyncLogger() {
        _running = false;
        if (_thread.joinable()) _thread.join();
    }

    void log(const LogEntry& entry) {
        if (entry.level >= _min_level) {
            _buffer.push(entry);
        }
    }

private:
    void run() {
        while (_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto batch = _buffer.flush();
            for (const auto& entry : batch) {
                printf("[%s][%s][%s][tid:%lu] %s() status=%d time=%lldms\n",
                    entry.timestamp.c_str(),
                    levelToString(entry.level),
                    entry.module.c_str(),
                    entry.thread_id,
                    entry.func.c_str(),
                    entry.status,
                    entry.duration_ms);
            }
        }
    }

    RingLogBuffer _buffer;
    LogLevel _min_level;
    std::atomic<bool> _running;
    std::thread _thread;
};

// RAII 日志器
class ScopedLogger {
public:
    ScopedLogger(const std::string& func, const std::string& module, LogLevel lvl, AsyncLogger& logger)
        : _func(func), _module(module), _level(lvl), _logger(logger), _start(std::chrono::steady_clock::now()) {}

    void setStatus(int status) { _status = status; }

    ~ScopedLogger() {
        auto end = std::chrono::steady_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
        _logger.log(LogEntry {
            _level, _func, _module, _status, duration,
            current_time_str(), get_thread_id()
        });
    }

private:
    std::string _func, _module;
    LogLevel _level;
    int _status = 0;
    std::chrono::steady_clock::time_point _start;
    AsyncLogger& _logger;
};

// 示例
AsyncLogger global_logger(LogLevel::INFO);

void test_func() {
    ScopedLogger logger("test_func", "TestModule", LogLevel::INFO, global_logger);
    std::this_thread::sleep_for(std::chrono::milliseconds(42));
    logger.setStatus(0);
}

int main() {
    test_func();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
