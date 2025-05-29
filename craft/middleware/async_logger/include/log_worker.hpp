#pragma once

#include <thread>
#include <atomic>
#include <condition_variable>
#include <string>
#include <queue>
#include "ring_buffer.hpp"

class LogWorker {
public:
    static LogWorker& GetInstance();

    void start();
    void stop();
    void submitLog(const std::string &msg);
    
private:
    LogWorker();
    ~LogWorker();

    void run();

    std::thread worker_;
    std::atomic<bool> running_;
    RingBuffer<std::string> ring_;
    std::condition_variable cv_;
    std::mutex mtx_;
};