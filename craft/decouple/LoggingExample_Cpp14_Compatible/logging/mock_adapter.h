#pragma once

#include "logging_adapter.h"
#include <gmock/gmock.h>

class MockLoggingAdapter : public ILoggingAdapter {
public:
    MOCK_METHOD(void, LogVector, (int level, int code, const std::vector<std::string>& args), ());

protected:
    void DoLog(int level, int code, const std::vector<std::string>& args) override {
        LogVector(level, code, args);
    }
};