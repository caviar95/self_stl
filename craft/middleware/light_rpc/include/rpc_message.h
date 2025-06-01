#pragma once

#include <string>

// struct RpcMessage {
//     uint32_t msg_id;
//     std::string service_name;
//     std::string method_name;
//     bool is_request;
//     std::string error;

//     std::string meta_json;
//     std::string body;
// };

struct RPCMessage {
    uint64_t request_id{};
    std::string method_name;
    std::string payload;
    bool is_request{true};
    int status_code{};
};
