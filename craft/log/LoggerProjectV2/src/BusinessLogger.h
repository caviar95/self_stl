#pragma once
#include "ILogger.h"
#include "PlatformLogger.h"
#include <utility>

class BusinessLogger : public ILogger {
public:
    explicit BusinessLogger(PlatformLogger& logger) : logger_(logger) {}

    template<typename... Args>
    void Log(int mode, Args&&... args) {
        logger_.Log(mode, std::forward<Args>(args)...);
    }

    void LogRaw(int mode, const std::string& message) override {
        std::cout << "[RawLog] mode=" << mode << ": " << message << std::endl;
    }

private:
    PlatformLogger& logger_;
};
