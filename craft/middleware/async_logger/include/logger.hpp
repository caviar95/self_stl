#pragma once

#include <string>
#include <chrono>
#include <sstream>
// #include <stdarg.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <iomanip>
#include <ctime>

#include "log_worker.hpp"
#include "log_level.h"

class Logger {
public:

    ~Logger() {
        Stop();
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& GetInstance() {
        static Logger instance{};
        return instance;
    }

    void Init(LogLevel level = LogLevel::INFO) {
        curLevel_ = level;

        LogWorker::GetInstance().start();
    }

    void Log(LogLevel level, const std::string& module, const std::string& msg) {
        if (level < curLevel_ || curLevel_ == LogLevel::NONE) {
            return;
        }

        std::ostringstream oss;
        auto now = std::chrono::system_clock::now();
        // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        auto t = std::chrono::system_clock::to_time_t(now);
        // std::tm tm{};
        // localtime_r(&t, &tm);

        // oss << "[" << std::put_time(&tm, "%F %T") << "." << std::setw(3) << std::setfill('0') << ms.count()
        //     << "][" << LogLevelToStr(level)
        //     << "][" << module
        //     << "][TID " << std::this_thread::get_id() << "] " << msg;

        oss << std::put_time(std::localtime(&t), "%F %T");
        oss << " [" << module << "] " << msg;
        LogWorker::GetInstance().submitLog(oss.str());
    }

    void Stop() {
        LogWorker::GetInstance().stop();
    }

    void SetLevel(LogLevel level) {
        curLevel_ = level;
    }

private:
    Logger() : curLevel_(LogLevel::INFO) {}

    LogLevel curLevel_;
    // std::unique_ptr<LogWorker> worker_;
};
