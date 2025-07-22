#include "ILogger.hpp"
#include "LoggerDispatcher.hpp"
#include "PlatformAPI.hpp"

class ProductLoggerImpl : public ILogger {
public:
    ProductLoggerImpl() = default;

protected:
    void ReportNoArgs(bool mode, int code) override {
        ReportError(mode, code);  // 无参数处理
    }

    void ReportVariadicForward(bool mode, int code) override {
        ReportError(mode, code);  // 兜底无参数处理（可不写）
    }

    template <typename T1, typename... Rest>
    void ReportVariadicForward(bool mode, int code, T1&& first, Rest&&... rest) {
        ReportError(mode, code, std::forward<T1>(first), std::forward<Rest>(rest)...);
    }
};

// 自动注册器
static ProductLoggerImpl logger;
struct AutoRegister {
    AutoRegister() {
        LoggerRegistry::Register(&logger);
    }
} _autoRegister;
