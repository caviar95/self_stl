#pragma once

#include <intf_log_sink.h>
#include <iostream>

class ConsoleSink : public ILogSink {
public:
    void Write(LogLevel level, const std::string& msg) override {
        std::cout << "[Console] [" << static_cast<int>(level) << "] " << msg << std::endl;
    }
};
