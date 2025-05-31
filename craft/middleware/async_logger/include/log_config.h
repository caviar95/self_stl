#pragma once
#include <string>
#include "log_level.h"

enum class LogDropStrategy {
    Drop,   // 丢弃日志
    Block,  // 阻塞等待
    Yield   // 自旋等待
};

class LogConfig {
public:
    static LogConfig& GetInstance();

    void SetLogPath(const std::string& path);
    void SetStrategy(LogDropStrategy strategy);
    void SetConsoleOutput(bool enable);
    void SetLogLevel(LogLevel level);

    std::string GetLogPath() const;
    bool GetConsoleOuputFlag() const;
    LogLevel GetLogLevel() const;
    LogDropStrategy GetLogDropStrategy() const;

private:
    LogConfig() = default;

    std::string logPath_;
    LogDropStrategy strategy_ = LogDropStrategy::Yield;
    bool consoleOutput_{true};
    LogLevel level_{LogLevel::INFO};
};
