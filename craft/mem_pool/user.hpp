#pragma once

#include "pooled_object.hpp"

class User : public PooledObject<User> {
private:
    int id_;
    std::string name_;

public:
    User(int id, const std::string& name) : id_(id), name_(name) {
        std::cout << "User constructor: ID=" << id_ << ", Name=" << name_ << "\n";
    }

    ~User() {
        std::cout << "User destrutor: ID=" << id_ << "\n";
    }

    void print() const {
        std::cout << "User ID: " << id_ << ", Name: " << name_ << "\n";
    }

    // 支持赋值操作（用于 update）
    User& operator=(const User& other) {
        id_ = other.id_;
        name_ = other.name_;
        return *this;
    }
};