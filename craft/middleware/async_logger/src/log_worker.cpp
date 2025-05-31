#include "log_worker.hpp"
#include "log_config.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>


LogWorker::LogWorker() : running_(false), writer_(LogConfig::GetInstance().GetLogPath()) {}

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
    // cv_.notify_all();

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
    LogDropStrategy strategy = LogConfig::GetInstance().GetLogDropStrategy();

    if (strategy == LogDropStrategy::Drop) {
        ring_.Push(msg); // 不管是否成功，直接返回
        return;
    }

    while (!ring_.Push(msg)) {
        if (strategy == LogDropStrategy::Block) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        } else {
            std::this_thread::yield();
        }
        
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
    std::string msg;
    while (running_) {
        if (ring_.Pop(msg)) {
            buffer_.Add(msg);
        } else {
            auto logs = buffer_.CollectAndReset();
            if (!logs.empty()) {
                writer_.Write(logs);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    auto remaining = buffer_.CollectAndReset();
    if (!remaining.empty()) {
        writer_.Write(remaining);
    }
}
