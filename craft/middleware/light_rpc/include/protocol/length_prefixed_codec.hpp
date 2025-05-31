#pragma once

#include "codec_intf.hpp"

#include <cstring>
#include <cstdint>

class LengthPrefixedCodec : public ICodec {
public:
    std::string Encode(const std::string& payload) override {
        uint32_t len = payload.size();
        std::string encoded;
        encoded.resize(sizeof(uint32_t) + len);

        std::memcpy(&encoded[0], &len, sizeof(uint32_t));
        std::memcpy(&encoded[sizeof(uint32_t)], payload.data(), len);

        return encoded;
    }

    bool Decode(const std::string& buffer, std::string& result, std::string& remaining) override {
        if (buffer.size() < sizeof(uint32_t)) {
            return false;
        }

        uint32_t len{};
        std::memcpy(&len, buffer.data(), sizeof(uint32_t));

        if (buffer.size() < sizeof(uint32_t) + len) {
            return false;
        }

        result.assign(buffer.begin() + sizeof(uint32_t), buffer.begin() + sizeof(uint32_t) + len);
        remaining.assign(buffer.begin() + sizeof(uint32_t) + len, buffer.end());

        return true;
    }
};