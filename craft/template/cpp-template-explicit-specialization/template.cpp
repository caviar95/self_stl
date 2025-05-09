// template.cpp
#include "template.hpp"
#include <iostream>

// 通用模板定义
template <typename T>
MyTemplateClass<T>::MyTemplateClass(T value) : data(value) {}

template <typename T>
void MyTemplateClass<T>::printData() {
    std::cout << "Generic type: " << data << std::endl;
}

// 显式特化定义
template <>
MyTemplateClass<int>::MyTemplateClass(int value) : data(value) {}

template <>
void MyTemplateClass<int>::printData() {
    std::cout << "Specialized int type: " << data << std::endl;
}

template <>
MyTemplateClass<double>::MyTemplateClass(double value) : data(value) {}

template <>
void MyTemplateClass<double>::printData() {
    std::cout << "Specialized double type: " << data << std::endl;
}

template <>
MyTemplateClass<long long>::MyTemplateClass(long long value) : data(value) {}

template <>
void MyTemplateClass<long long>::printData() {
    std::cout << "Specialized long long type: " << data << std::endl;
}    