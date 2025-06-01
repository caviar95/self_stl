#include "../include/rpc_message_codec.hpp"
#include <iostream>
#include <cassert>

int main() {
    RPCMessageCodec codec;

    RPCMessage req;
    req.request_id = 123;
    req.method_name = "add";
    req.payload = "{\"a\":1,\"b\":2}";
    req.is_request = true;

    std::string encoded = codec.serialize(req);
    RPCMessage decoded = codec.deserialize(encoded);

    assert(decoded.request_id == 123);
    assert(decoded.method_name == "add");
    assert(decoded.payload == "{\"a\":1,\"b\":2}");
    assert(decoded.is_request);

    std::cout << "RPCMessageCodec test passed." << std::endl;
    return 0;
}