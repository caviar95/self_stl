#pragma once

#include <vector>
#include <sstream>
#include <iostream>

// 通用版本（需要C++11）
#define PRINT(...) \
    do { \
        std::cout << print_wrapper(#__VA_ARGS__, __VA_ARGS__) << std::endl; \
    } while(0)

// 字符串分割工具函数
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token.erase(0, token.find_first_not_of(' '))); // 去除空格
    }
    return tokens;
}

template<typename... Args>
std::string print_wrapper(const char* names, const Args&... args) {
    std::stringstream ss;
    std::string names_str(names);
    std::vector<std::string> tokens = split(names_str, ',');
    
    size_t i = 0;
    ((ss << tokens[i++] << " = " << args << (i < sizeof...(Args) ? ", " : "")), ...);
    
    return ss.str();
}
