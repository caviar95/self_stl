#pragma once

#include <string>

class ICodec {
public:
    virtual ~ICodec() = default;
    virtual std::string Encode(const std::string &payload) = 0;

    virtual bool Decode(const std::string& buffer, std::string &result, std::string &remaining) = 0;
};