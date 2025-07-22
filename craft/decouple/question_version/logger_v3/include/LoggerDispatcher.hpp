#pragma once
#include "ILogger.hpp"

class LoggerRegistry {
public:
    static void Register(ILogger* logger) {
        Instance() = logger;
    }

    static ILogger* Get() {
        return Instance();
    }

private:
    static ILogger*& Instance() {
        static ILogger* instance = nullptr;
        return instance;
    }
};

template <typename... Args>
inline void DispatchReport(bool mode, int code, Args&&... args) {
    if (auto* logger = LoggerRegistry::Get()) {
        logger->Report(mode, code, std::forward<Args>(args)...);
    }
}
