#pragma once

#include "codec_intf.hpp"
#include <cstdint>
#include <cstring>

class TLVCodec : public ICodec {
public:
    std::string Encode(const std::string& payload) override {
        uint8_t type = 1;
        uint32_t length = payload.size();

        std::string encoded;
        encoded.resize(1 + sizeof(uint32_t) + length);
        encoded[0] = type;
        std::memcpy(&encoded[1], &length, sizeof(uint32_t));
        std::memcpy(&encoded[1 + sizeof(uint32_t)], payload.data(), length);

        return encoded;
    }

    bool Decode(const std::string& buffer, std::string &result, std::string &remaining) override {
        if (buffer.size() < 1 + sizeof(uint32_t)) {
            return false;
        }

        uint8_t type = buffer[0];
        if (type != 1) {
            return false;
        }

        uint32_t length{};
        std::memcpy(&length, &buffer[1], sizeof(uint32_t));

        if (buffer.size() < 1 + sizeof(uint32_t) + length) {
            return false;
        }

        result.assign(buffer.begin() + 1 + sizeof(uint32_t), buffer.begin() + 1 + sizeof(uint32_t) + length);
        remaining.assign(buffer.begin() + 1 + sizeof(uint32_t) + length, buffer.end());

        return true;
    }
};