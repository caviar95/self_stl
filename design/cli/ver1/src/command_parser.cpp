#include "../include/command_parser.h"

#include <sstream>

std::vector<std::string> CommandParser::tokenize(const std::string& input) {
    std::istringstream iss(input);
    std::string token;
    std::vector<std::string> tokens;

    while (iss >> token) tokens.emplace_back(token);

    return tokens;
} 

std::unordered_map<std::string, std::string> CommandParser::extractOptions(
        const std::vector<std::string>& tokens, size_t start) {
    std::unordered_map<std::string, std::string> options;

    for (size_t i = start; i + 1 < tokens.size(); i += 2) {
        options[tokens[i]] = tokens[i + 1];
    }

    return options;
}
