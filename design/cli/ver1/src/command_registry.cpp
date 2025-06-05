#include "../include/command_registry.h"
#include <iostream>

void CommandRegistry::RegisterCommand(const std::vector<std::string>& path,
        std::function<void(const std::unordered_map<std::string, std::string>&)> handler) {
    CommandNode *node = &root_;
    for (const auto& part : path) {
        node = &node->children[part];
        node->name = part;
    }

    node->handler = handler;
}

void CommandRegistry::Dispatch(const std::string& input) {
    auto tokens = parser_.tokenize(input);
    if (tokens.empty()) {
        return;
    }

    std::unordered_map<std::string, std::string> options = 
        parser_.extractOptions(tokens, 2);

    DispatchRecursive(root_, tokens, 0, options);
}

void CommandRegistry::DispatchRecursive(const CommandNode& node,
                               const std::vector<std::string>& tokens,
                               size_t index,
                               const std::unordered_map<std::string, std::string> &options) {
    if (index >= tokens.size()) return;

    auto it = node.children.find(tokens[index]);
    if (it != node.children.end()) {
        const auto& child = it->second;
        if (child.handler && index == tokens.size() - 3) {
            child.handler(options);
        } else {
            DispatchRecursive(child, tokens, index + 1, options);
        }
    } else {
        std::cerr << "Unknown command: " << tokens[index] << "\n";
    }
}
