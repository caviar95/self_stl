// 📁 项目结构模板
// ------------------
// threadpool_system/
// ├── include/
// │   ├── thread_pool.hpp          // 支持优先级、绑定线程、取消、调度时间
// │   ├── task.hpp                 // Task 对象，封装功能、优先级、取消、调度时间等
// │   ├── scheduler.hpp            // 定时任务调度器
// │   ├── logger.hpp               // 日志模块
// │   ├── monitor.hpp              // 运行时状态监控
// ├── src/
// │   ├── thread_pool.cpp
// │   ├── scheduler.cpp
// │   ├── logger.cpp
// │   ├── monitor.cpp
// ├── tests/
// │   ├── test_thread_pool.cpp     // 单元测试示例
// ├── main.cpp                     // 示例主程序
// ├── CMakeLists.txt               // 构建脚本

// -------------------------
// include/logger.hpp
#pragma once
#include <mutex>
#include <iostream>
#include <string>
#include <chrono>

class Logger {
public:
    static Logger& instance();
    void log(const std::string& msg);

private:
    Logger() = default;
    std::mutex log_mutex;
};

// -------------------------
// src/logger.cpp
#include "logger.hpp"
Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(log_mutex);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << std::ctime(&now) << ": " << msg << std::endl;
}

// -------------------------
// include/monitor.hpp
#pragma once
#include <atomic>
#include <vector>

class Monitor {
public:
    void task_enqueued();
    void task_completed();
    void thread_started();
    void thread_exited();

    int get_pending_tasks() const;
    int get_active_threads() const;

private:
    std::atomic<int> pending_tasks{0};
    std::atomic<int> active_threads{0};
};

// -------------------------
// src/monitor.cpp
#include "monitor.hpp"

void Monitor::task_enqueued() {
    pending_tasks++;
}

void Monitor::task_completed() {
    pending_tasks--;
}

void Monitor::thread_started() {
    active_threads++;
}

void Monitor::thread_exited() {
    active_threads--;
}

int Monitor::get_pending_tasks() const {
    return pending_tasks.load();
}

int Monitor::get_active_threads() const {
    return active_threads.load();
}
