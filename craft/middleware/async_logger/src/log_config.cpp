// log_config.cpp
#include "log_config.h"

LogConfig& LogConfig::get() {
    static LogConfig instance;
    return instance;
}

void LogConfig::setLogPath(const std::string& path) {
    logPath = path;
}

void LogConfig::setStrategy(LogDropStrategy s) {
    strategy = s;
}

void LogConfig::setConsoleOutput(bool enable) {
    consoleOutput = enable;
}
