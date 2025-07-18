#include "user_module.h"
#include "../logging/logging_adapter.h"

ILoggingAdapter* gLogger = nullptr;

void UserModuleFunction() {
    gLogger->Log(1, 0x1234, 42);
    gLogger->Log(2, 0x2345, "example", "test");
}