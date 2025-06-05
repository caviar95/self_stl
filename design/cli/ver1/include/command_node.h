#pragma once

#include <string>
#include <functional>
#include <unordered_map>

struct CommandNode {
    std::string name;
    std::unordered_map<std::string, CommandNode> children;
    std::function<void(const std::unordered_map<std::string, std::string>&)> handler;

    CommandNode(const std::string &n = "") : name(n) {}
};

