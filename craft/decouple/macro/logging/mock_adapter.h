#pragma once

#include "logging_adapter.h"
#include <gmock/gmock.h>

class MockLoggingAdapter : public ILoggingAdapter {
public:
    // mockable 版本（固定签名）
    MOCK_METHOD(void, LogVector, (int level, int code, const std::vector<std::string>& args), ());

protected:
    // override DoLog，转发到 mock 方法
    void DoLog(int level, int code, const std::vector<std::string>& args) override {
        LogVector(level, code, args);
    }
};
