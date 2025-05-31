#pragma once

#include <string>

enum class LogLevel {
    INFO,
    WARN,
    ERROR,
    NONE
};

inline const char* LogLevelToStr(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
