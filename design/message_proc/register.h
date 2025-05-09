#pragma once

#include "message_dispatcher.h"

#define REGISTER_HANDLE(type, Class) \
class Class##Registry { \
public: \
    Class##Registry() { \
        MessageDispatcher::Instance().RegisterHandler(type, std::make_unique<Class>()); \
    } \
}; \

// 使用举例：
// REGISTER_HANDLE(MessageType::Type1, HandlerClass);