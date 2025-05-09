#pragma once

#include <unordered_map>
#include <memory>

#include "message_handler.h"

class VersionedParser : public IMessageHandler {
    std::unordered_map<Version, std::unique_ptr<IParserImpl>> parses_;

public:
    ParsedResult Parse(MessageContext& ctx) override {
        auto version = ctx.GetHeader().version;
        auto it = parses_.find(version);
        if (it != parses_.end()) {
            return it->second->Parse(ctx);
        }

        return FindCompatibleParser(version)->Parse(ctx);
    }
};