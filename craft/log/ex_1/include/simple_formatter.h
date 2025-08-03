#pragma once

#include "intf_log_formatter.h"
#include <sstream>

class SimpleFormatter : public ILogFormatter {
protected:
    std::string FormatImpl(LogLevel level, const std::string& pattern,
                           const std::vector<std::string>& args) override {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(level) << "] " << pattern;
        for (const auto& arg : args) {
            oss << " " << arg;
        }
        return oss.str();
    }
};
