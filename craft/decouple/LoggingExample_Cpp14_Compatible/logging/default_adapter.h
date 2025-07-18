#pragma once

#include "logging_adapter.h"
#include "../platform/platform.h"

class DefaultLoggingAdapter : public ILoggingAdapter {
public:
    void DoLog(int level, int code, const std::vector<std::string>& args) override {
        CallPrintFromVector(level, code, args, 0);
    }

private:
    template <typename... Args>
    void CallPrint(int level, int code, Args&&... args) {
        Print(level, code, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void CallPrintFromVector(int level, int code, const std::vector<std::string>& vec, size_t index, Args&&... args) {
        if (index == vec.size()) {
            CallPrint(level, code, std::forward<Args>(args)...);
        } else {
            CallPrintFromVector(level, code, vec, index + 1, std::forward<Args>(args)..., vec[index]);
        }
    }
};