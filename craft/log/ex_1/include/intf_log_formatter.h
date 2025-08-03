#pragma once

#include <string>
#include "log_level.h"
#include <vector>
#include <sstream>

class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;

    template<typename... Args>
    std::string Format(LogLevel level, const std::string& pattern, Args&&... args) {
        std::vector<std::string> argVec = { ToString(std::forward<Args>(args))... };
        return FormatImpl(level, pattern, argVec);
    }

protected:
    virtual std::string FormatImpl(LogLevel level, const std::string& pattern,
                                   const std::vector<std::string>& args) = 0;

private:
    template<typename T>
    std::string ToString(T&& val) {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }
};
