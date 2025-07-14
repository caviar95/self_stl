#pragma once
#include <iostream>
#include <memory>

struct ILogger {
    virtual ~ILogger() = default;
    virtual void Log(const std::string& message) = 0;
};

class ProductLogger : public ILogger {
public:
    void Log(const std::string& message) override {
        std::cout << "[ProductLogger] " << message << std::endl;
    }
};
