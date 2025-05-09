// 统一消息处理器接口

#pragma once

#include <memory>

#include "base_type.h"

class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;

    // main parse function
    virtual ParsedResult Parse(const MessageContext &ctx) = 0;

    // dynamic register support
    static std::unique_ptr<IMessageHandler> Create(int type);

    // meta info interface
    virtual const MessageMeta &GetMeta() const = 0;
};
