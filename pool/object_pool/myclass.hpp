#pragma once


#include <iostream>

class MyClass {
    int id;
    static int nextId;
public:
    MyClass() : id(nextId++) {
        std::cout << "MyClass 实例已创建，id: " << id << std::endl;
    }

    MyClass(const MyClass& other) : id(nextId++), value(other.value) {}

    void doSomething() {
        std::cout << "MyClass 正在执行操作，id: " << id << std::endl;
    }

    void setValue(int value) {
        this->value = value;
    }

    int getValue() const {
        return value;
    }

    int getId() const {
        return id;
    }

private:
    int value = 0;
};

int MyClass::nextId = 0;
    