#pragma once
#include <string>

enum class LogDropStrategy {
    Drop,   // 丢弃日志
    Block,  // 阻塞等待
    Yield   // 自旋等待
};

class LogConfig {
public:
    static LogConfig& get();

    void setLogPath(const std::string& path);
    void setStrategy(LogDropStrategy strategy);
    void setConsoleOutput(bool enable);

    std::string logPath;
    LogDropStrategy strategy = LogDropStrategy::Yield;
    bool consoleOutput = true;

private:
    LogConfig() = default;
};
