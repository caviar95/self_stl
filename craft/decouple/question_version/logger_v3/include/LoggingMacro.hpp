#pragma once
#include "LoggerDispatcher.hpp"

#define REPORT(mode, code, ...) \
    DispatchReport(mode, code, ##__VA_ARGS__)
