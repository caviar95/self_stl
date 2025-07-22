#pragma once
#include <utility>
#include <cstddef>

class ILogger {
public:
    virtual ~ILogger() = default;

    // 外部统一调用接口（支持无参/有参）
    template <typename... Args>
    void Report(bool mode, int code, Args&&... args) {
        if constexpr (sizeof...(Args) == 0) {
            ReportNoArgs(mode, code);
        } else {
            ReportWithArgs(mode, code, std::forward<Args>(args)...);
        }
    }

protected:
    // 子类必须实现：无参数版本
    virtual void ReportNoArgs(bool mode, int code) = 0;

    // 子类必须实现：有参数版本（用模板转发）
    virtual void ReportVariadic(bool mode, int code) = 0;

    // 模板转发函数，不是虚函数
    template <typename T1, typename... Rest>
    void ReportWithArgs(bool mode, int code, T1&& first, Rest&&... rest) {
        ReportVariadicImpl(mode, code, std::forward<T1>(first), std::forward<Rest>(rest)...);
    }

private:
    // 非虚模板函数，供上层分发调用
    template <typename T1, typename... Rest>
    void ReportVariadicImpl(bool mode, int code, T1&& first, Rest&&... rest) {
        ReportVariadicForward(mode, code, std::forward<T1>(first), std::forward<Rest>(rest)...);
    }

protected:
    // 由子类 override，用于处理参数展开转发
    virtual void ReportVariadicForward(bool mode, int code) = 0;

    template <typename T1, typename... Rest>
    void ReportVariadicForward(bool mode, int code, T1&& first, Rest&&... rest) {
        // 可选：提供默认处理逻辑
    }
};
