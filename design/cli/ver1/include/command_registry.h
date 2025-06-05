#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <string>

#include "command_node.h"
#include "command_parser.h"

class CommandRegistry {
public:
    void RegisterCommand(const std::vector<std::string> &path,
            std::function<void(const std::unordered_map<std::string, std::string>&)> handler);
    void Dispatch(const std::string& input);

private:
    CommandNode root_;
    CommandParser parser_;

    void DispatchRecursive(const CommandNode& node,
                           const std::vector<std::string>& tokens,
                           size_t index,
                           const std::unordered_map<std::string, std::string>& options);
};

