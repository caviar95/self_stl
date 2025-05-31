// log_config.cpp
#include "log_config.h"

LogConfig& LogConfig::GetInstance() {
    static LogConfig instance;
    return instance;
}

void LogConfig::SetLogPath(const std::string& path) {
    logPath_ = path;
}

void LogConfig::SetStrategy(LogDropStrategy s) {
    strategy_ = s;
}

void LogConfig::SetConsoleOutput(bool enable) {
    consoleOutput_ = enable;
}

void LogConfig::SetLogLevel(LogLevel level) {
    level_ = level;
}

std::string LogConfig::GetLogPath() const
{
    return logPath_;
}

bool LogConfig::GetConsoleOuputFlag() const
{
    return consoleOutput_;
}

LogLevel LogConfig::GetLogLevel() const
{
    return level_;
}

LogDropStrategy LogConfig::GetLogDropStrategy() const
{
    return strategy_;
}
