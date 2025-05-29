#pragma once

#include <string>
#include <chrono>
#include <sstream>
#include <stdarg.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <iomanip>

enum class LogLevel {
    INFO, WARN, ERROR,
};

class Logger {
public:

    ~Logger() {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& GetInstance() {
        static Logger instance{};
        return instance;
    }

    void Log(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
        if (level < curLevel_) {
            return;
        }

        char buffer[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        std::ostringstream oss;
        oss << "[" << GetCurrentTimeStamp() << "] "
            << "[" << GetThreadId() << "] "
            << "[" << file << ":" << line << "] "
            << "[" << (level == LogLevel::INFO ? "INFO" : (level == LogLevel::WARN ? "WARN" : "ERROR")) << "] "
            << buffer;

        LogImpl(level, oss.str());
    }
    
    void SetLevel(LogLevel level) {
        curLevel_ = level;
    }

private:
    Logger() : curLevel_(LogLevel::INFO) {}

    LogLevel curLevel_;
    std::mutex mutex_;

    void LogImpl(LogLevel level, const std::string &msg) {
        std::lock_guard<std::mutex> lk(mutex_);
        std::cout << msg << std::endl;
    }

    std::string GetCurrentTimeStamp() {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&t), "%F %T")
            << "." << std::setw(3) << std::setfill('0') << ms.count();

        return oss.str();
    }
    std::string GetThreadId() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
        // return std::to_string(std::this_thread::get_id());
    }
};

#define LOG_INFO(fmt, ...) Logger::GetInstance().Log(LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Logger::GetInstance().Log(LogLevel::WARN, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::GetInstance().Log(LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
