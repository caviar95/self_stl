#pragma once

#include "logging_adapter.h"
#include "../platform/platform.h"

class DefaultLoggingAdapter : public ILoggingAdapter {
public:
    void DoLog(int level, int code, const std::vector<std::string>& args) override {
        // 可选调试输出
        for (const auto& s : args) std::cout << "[Default] " << s << " ";
        std::cout << std::endl;

        // 若 Print 支持 vector<string>，可传入；否则你需要自定义展开
        // 简单展开最多支持3参数（演示用）
        switch (args.size()) {
            case 0: Print(level, code); break;
            case 1: Print(level, code, args[0]); break;
            case 2: Print(level, code, args[0], args[1]); break;
            case 3: Print(level, code, args[0], args[1], args[2]); break;
            default: std::cout << "[Default] Too many arguments." << std::endl;
        }
    }
};
