#pragma once
#include "ArgList.hpp"

class ILogger {
public:
    virtual ~ILogger() = default;
    // DispatchReport 会统一调用这一个接口
    virtual void ReportImpl(bool mode, int code, IArgList* args) = 0;
};
