#pragma once
#include <string>

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void LogRaw(int mode, const std::string& message) = 0;
};
