#pragma once
#include <iostream>
#include <string>
#include <map>
#include <sstream>

class PlatformLogger {
public:
    PlatformLogger() {
        formatMap[0] = "INT: {}, STR: {}, HEX: {:#x}";
        formatMap[1] = "MSG: {} {} {}";
        formatMap[2] = "NO PARAMS LOG";
    }

    template<typename... Args>
    void Log(int mode, Args&&... args) {
        auto fmt = GetFormat(mode);
        auto msg = Format(fmt, std::forward<Args>(args)...);
        std::cout << "[PlatformLog] " << msg << std::endl;
    }

private:
    std::map<int, std::string> formatMap;

    const std::string& GetFormat(int mode) {
        auto it = formatMap.find(mode);
        if (it != formatMap.end()) {
            return it->second;
        }
        static std::string unknown = "UNKNOWN MODE";
        return unknown;
    }

    template<typename... Args>
    std::string Format(const std::string& fmt, Args&&... args) {
        std::ostringstream oss;
        AppendArgs(oss, std::forward<Args>(args)...);
        return oss.str();
    }

    void AppendArgs(std::ostringstream&) {}

    template<typename T, typename... Rest>
    void AppendArgs(std::ostringstream& oss, T&& val, Rest&&... rest) {
        oss << val << " ";
        AppendArgs(oss, std::forward<Rest>(rest)...);
    }
};
