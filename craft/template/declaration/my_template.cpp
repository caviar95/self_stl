#include "my_template.hpp"
#include <cmath>
#include <iostream>

// ===== 非特化成员定义 =====
// template <typename T>
// MyClass<T>::MyClass(T value) : data(value) {}

// template <typename T>
// void MyClass<T>::print() const {
//     std::cout << "[Generic] " << data << std::endl;
// }

// ===== int特化成员定义 =====
MyClass<int>::MyClass(int value) : data(value * 2) {}  // 构造函数定义

void MyClass<int>::print() const {
    std::cout << "[Int] " << data << std::endl;
}

// ===== double特化成员定义 =====
MyClass<double>::MyClass(double value) : data(value / 2) {}

void MyClass<double>::print() const {
    std::cout << "[Double] " << data << std::endl;
}

// ===== long long特化成员定义 =====
MyClass<long long>::MyClass(long long value) : data(value + 1000) {}

void MyClass<long long>::print() const {
    std::cout << "[LongLong] " << data << std::endl;
}