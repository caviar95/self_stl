#pragma once

#include <vector>
#include <string>
#include <sstream>

class ILoggingAdapter {
public:
    virtual ~ILoggingAdapter() = default;

    // 统一入口（业务代码使用）
    template <typename... Args>
    void Log(int level, int code, Args&&... args) {
        DoLog(level, code, ToStringVector(std::forward<Args>(args)...));
    }

protected:
    // 实现类必须实现
    virtual void DoLog(int level, int code, const std::vector<std::string>& args) = 0;

    // 类型转换工具
    template <typename T>
    std::string ToString(const T& val) const {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }

    std::string ToString(const std::string& s) const { return s; }
    std::string ToString(const char* s) const { return std::string(s); }

    template <typename... Args>
    std::vector<std::string> ToStringVector(Args&&... args) const {
        return { ToString(std::forward<Args>(args))... };
    }
};
