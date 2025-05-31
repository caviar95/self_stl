#include "../include/protocol/length_prefixed_codec.hpp"
#include "../include/protocol/tlv_codec.hpp"
#include <iostream>
#include <cassert>

void testCodec(ICodec& codec, const std::string& name) {
    std::string payload = "Hello RPC Protocol";
    std::string encoded = codec.Encode(payload);

    std::string decoded;
    std::string leftover;

    bool ok = codec.Decode(encoded, decoded, leftover);

    std::cout << "[" << name << "] ";
    if (!ok) {
        std::cerr << "Decode failed!" << std::endl;
        return;
    }

    assert(decoded == payload);
    assert(leftover.empty());

    std::cout << "Test Passed: '" << decoded << "'" << std::endl;
}

int main() {
    LengthPrefixedCodec lengthCodec;
    TLVCodec tlvCodec;

    testCodec(lengthCodec, "LengthPrefixedCodec");
    testCodec(tlvCodec, "TLVCodec");

    return 0;
}