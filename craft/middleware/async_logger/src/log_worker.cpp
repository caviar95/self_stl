#include "log_worker.hpp"

#include <iostream>
#include <chrono>

constexpr int RING_SIZE = 8192;
LogWorker::LogWorker() : running_(false), ring_(RING_SIZE) {}

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

void LogWorker::submitLog(const std::string &msg) {
    while (!ring_.push(msg)) {
        std::this_thread::yield();
    }

    cv_.notify_one();
}

void LogWorker::run() {
    while (running_) {
        std::unique_lock<std::mutex> lk(mtx_);
        cv_.wait_for(lk, std::chrono::milliseconds(100));

        while (true) {
            auto item = ring_.pop();
            if (!item.has_value()) {
                break;
            }

            std::cout << item.value() << std::endl;
        }
    }
}
