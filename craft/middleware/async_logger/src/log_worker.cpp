#include "log_worker.hpp"
#include "log_config.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>


LogWorker::LogWorker() : running_(false) {}

LogWorker::~LogWorker() {
    stop();
}

LogWorker& LogWorker::GetInstance() {
    static LogWorker instance;
    return instance;
}

void LogWorker::start() {
    running_ = true;
    worker_ = std::thread(&LogWorker::run, this);
}

void LogWorker::stop() {
    running_ = false;
    cv_.notify_all();

    if (worker_.joinable()) {
        worker_.join();
    }
}

// void LogWorker::submitLog(const std::string &msg) {
//     while (!ring_.push(msg)) {
//         std::this_thread::yield();
//     }

//     cv_.notify_one();
// }

void LogWorker::submitLog(const std::string& msg) {
    LogDropStrategy strategy = LogConfig::get().strategy;

    if (strategy == LogDropStrategy::Drop) {
        ring_.push(msg); // 不管是否成功，直接返回
        return;
    }

    if (strategy == LogDropStrategy::Block) {
        while (!ring_.push(msg)) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        return;
    }

    // 默认 Yield
    while (!ring_.push(msg)) {
        std::this_thread::yield();
    }
}


// void LogWorker::run() {
//     while (running_) {
//         std::unique_lock<std::mutex> lk(mtx_);
//         cv_.wait_for(lk, std::chrono::milliseconds(100));

//         while (true) {
//             auto item = ring_.pop();
//             if (!item.has_value()) {
//                 break;
//             }

//             std::cout << item.value() << std::endl;
//         }
//     }
// }

void LogWorker::run() {
    std::ofstream outfile;
    if (!LogConfig::get().consoleOutput) {
        outfile.open(LogConfig::get().logPath, std::ios::app);
    }

    std::vector<std::string> batch;
    batch.reserve(256);

    while (running_) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait_for(lock, std::chrono::milliseconds(100));
        }

        // 批量弹出日志
        batch.clear();
        std::string msg;
        while (ring_.pop(msg)) {
            batch.push_back(msg);
        }

        if (batch.empty()) continue;

        for (auto& line : batch) {
            if (LogConfig::get().consoleOutput) {
                std::cout << line << std::endl;
            } else {
                outfile << line << std::endl;
            }
        }

        if (outfile) outfile.flush();
    }

    if (outfile) outfile.close();
}
