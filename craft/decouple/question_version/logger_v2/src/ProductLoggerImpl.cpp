#include "ILogger.hpp"
#include "LoggerDispatcher.hpp"
#include "../PlatformAPI.hpp"
#include <functional>

class ProductLoggerImpl : public ILogger {
public:
    ProductLoggerImpl() = default;

    void ReportImpl(bool mode, int code, IArgList* args) override {
        if (!args) {
            // 无参调用
            ReportError(mode, code);
        }
        else {
            // 带参：用类型擦除回调展开参数到 ReportError
            // 构造 std::function<void(Args...)> 的占位对象
            // IArgList::applyImpl 会 reinterpret_cast 回来
            using FnType = std::function<void()>;
            auto fnWrapper = new std::function<void()>(
                [mode, code]() {
                    // 这个 lambda 只作占位，真正展开在 ArgList
                }
            );
            // 其实 ArgList::applyImpl 会把 fnWrapper reinterpret_cast
            // 成 std::function<void(Args...)> 并调用它
            args->apply(*fnWrapper);
            delete fnWrapper;
        }
    }
};

// 自动注册到全局
static ProductLoggerImpl _loggerInstance;
struct AutoRegister {
    AutoRegister() {
        LoggerRegistry::Register(&_loggerInstance);
    }
} _autoRegister;
