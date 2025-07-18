#pragma once
#include <iostream>

template <typename... Args>
void Print(int level, int errCode, Args&&... args) {
    std::cout << "[Platform::Print] level=" << level
              << " errCode=0x" << std::hex << errCode
              << " args=[ ";
    (std::cout << ... << args) << " ]" << std::endl;
}