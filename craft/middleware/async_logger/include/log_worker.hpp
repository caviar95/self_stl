#pragma once

#include <thread>
#include <atomic>
#include <condition_variable>
#include <string>
#include <queue>
#include <thread>

#include "ring_buffer.hpp"
#include "flush_buffer.h"
#include "file_writer.h"
#include "log_config.h"

constexpr int RING_SIZE = 8192;

class LogWorker {
public:
    static LogWorker& GetInstance();

    void start();
    void stop();
    void submitLog(const std::string &msg);
    
    ~LogWorker();

private:
    LogWorker();

    void run();

    std::thread worker_;
    std::atomic<bool> running_;
    MPSCRingBuffer<std::string, RING_SIZE> ring_;
    std::condition_variable cv_;
    std::mutex mtx_;
    FlushBuffer buffer_;
    FileWriter writer_;
};