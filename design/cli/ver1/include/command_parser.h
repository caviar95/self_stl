#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class CommandParser {
public:
    std::vector<std::string> tokenize(const std::string& input);
    std::unordered_map<std::string, std::string> extractOptions(
                const std::vector<std::string>& tokens, size_t start);
};
