#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdarg>
#include <cstdio>

// 模拟平台内部错误码 -> 格式化字符串映射
inline const char* GetFormatByCode(int code) {
    static const std::unordered_map<int, const char*> formats = {
        {1001, "Disk %s full, usage %d%%"},
        {1002, "User %s not found at line %d"},
        {1003, "Floating point error: %e"},
        {9999, "Unknown error"}
    };
    auto it = formats.find(code);
    return it != formats.end() ? it->second : "[Unknown error code]";
}

// 平台提供的模板函数（不可修改）
template <typename... Args>
int ReportError(bool mode, int code, Args&&... args) {
    if (mode) return 0;
    const char* fmt = GetFormatByCode(code);
    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer), fmt, std::forward<Args>(args)...);
    std::cerr << "[Platform] " << buffer << std::endl;
    return -1;
}
