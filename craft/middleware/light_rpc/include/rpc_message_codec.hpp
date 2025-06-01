#pragma once

#include "rpc_message.h"
#include <nlohmann/json.hpp>

class RPCMessageCodec {
public:
    std::string serialize(const RPCMessage& msg) {
        nlohmann::json j;
        j["request_id"] = msg.request_id;
        j["method_name"] = msg.method_name;
        j["payload"] = msg.payload;
        j["is_request"] = msg.is_request;
        j["status_code"] = msg.status_code;
        return j.dump();
    }

    RPCMessage deserialize(const std::string& data) {
        nlohmann::json j = nlohmann::json::parse(data);
        RPCMessage msg;
        msg.request_id = j["request_id"];
        msg.method_name = j["method_name"];
        msg.payload = j["payload"];
        msg.is_request = j["is_request"];
        msg.status_code = j["status_code"];
        return msg;
    }
};