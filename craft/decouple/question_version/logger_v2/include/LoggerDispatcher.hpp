#pragma once
#include "ILogger.hpp"
#include "ArgList.hpp"

// 简单的全局注册器
class LoggerRegistry {
public:
    static void Register(ILogger* logger) { Instance() = logger; }
    static ILogger* Get() { return Instance(); }
private:
    static ILogger*& Instance() {
        static ILogger* instance = nullptr;
        return instance;
    }
};

// 无参重载
inline void DispatchReport(bool mode, int code) {
    if (auto* lg = LoggerRegistry::Get()) {
        lg->ReportImpl(mode, code, nullptr);
    }
}

// 有参重载
template <typename... Args>
inline void DispatchReport(bool mode, int code, Args&&... args) {
    if (auto* lg = LoggerRegistry::Get()) {
        // new 出 ArgList，内部会 self-delete
        lg->ReportImpl(
            mode, code,
            new ArgList<Args...>(std::forward<Args>(args)...)
        );
    }
}
