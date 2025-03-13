#pragma once

#include "pooled_object.hpp"

// 示例派生类
class User : public PooledObject {
private:
    int id_;
    std::string name_;

public:
    User(int id, const std::string& name) : id_(id), name_(name) {
        std::cout << "User Constructor: ID=" << id_ << ", Name=" << name_ << "\n";
    }

    ~User() {
        std::cout << "User Destructor: ID=" << id_ << "\n";
    }

    void print() const {
        std::cout << "User ID: " << id_ << ", Name: " << name_ << "\n";
    }
};
