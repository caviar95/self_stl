#pragma once

#include <string>
#include "log_level.h"

class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void Write(LogLevel level, const std::string& msg) = 0;
};

