// platform.h
#pragma once
#include <iostream>

template <typename... Args>
void Print(int level, int errCode, Args... args) {
    std::cout << "[Platform::Print] level=" << level << " errCode=" << std::hex << errCode << " args=[ ";
    (std::cout << ... << args) << " ]" << std::endl;
}

#define LOG(level, code, ...) Print(level, code, ##__VA_ARGS__)
