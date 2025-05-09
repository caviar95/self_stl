#pragma once

#include <string>
#include <iostream>

// 通用模板类
template <typename T>
class MyClass {
private:
    T data;
public:
    MyClass(T value) : data(value) {}
    void print() const {
        std::cout << "[Generic] " << data << std::endl;
    }
};

// ============ 特化类完整声明（包含成员函数声明）============
template <>
class MyClass<int> {
private:
    int data;
public:
    MyClass(int value);  // 声明构造函数
    void print() const;  // 声明成员函数
};

template <>
class MyClass<double> {
private:
    double data;
public:
    MyClass(double value);
    void print() const;
};

template <>
class MyClass<long long> {
private:
    long long data;
public:
    MyClass(long long value);
    void print() const;
};

// // 显式实例化常用类型
// template class MyClass<std::string>;  // 关键行