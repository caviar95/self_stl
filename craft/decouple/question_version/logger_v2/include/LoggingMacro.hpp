#pragma once
#include "LoggerDispatcher.hpp"

// 在无额外参数时，##__VA_ARGS__ 会自动移除多余逗号
#define REPORT(mode, code, ...) \
    DispatchReport(mode, code, ##__VA_ARGS__)
