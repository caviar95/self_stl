// 消息类型路由

#pragma once

#include "message_handler.h"

#include <unordered_map>

class MessageDispatcher {
    std::unordered_map<int, std::unique_ptr<IMessageHandler>> handlers_;

public:
    void registerHandler(int type, std::unique_ptr<IMessageHandler> handler) {
        handlers_[type] = std::move(handler);
    }

    ParsedResult Dispatch(const RawMessage& raw_msg) {
        auto it = handlers_.find(raw_msg.header.type);
        if (it == handlers_.end()) {
            return ParsedResult::Error;
        }

        return it->second->Parse(CreateContext(raw_msg));
    }
};