#pragma once

#include <string>

struct RawMessage {
    struct Header {
        int type;
    };

    Header header;
    std::string body;
};

struct MessageContext {};

MessageContext CreateContext(const RawMessage& raw_msg) {
    return MessageContext();
}

enum ParsedResult {
    Success,
    Error,
    Type1,
    Type2,
    Unknown
};

struct MessageMeta;

struct ParsedTable {};
