#pragma once

#include "pooled_object.hpp"

// 示例派生类
class User : public PooledObject {
private:
    int id_;
    std::string name_;

public:
    User(int id, const std::string& name) : id_(id), name_(name) {
        std::cout << "User constructed: ID=" << id_ << ", Name=" << name_ << "\n";
    }

    ~User() {
        std::cout << "User destroyed: ID=" << id_ << "\n";
    }

    void print() const {
        std::cout << "User ID: " << id_ << ", Name: " << name_ << "\n";
    }
};

// 初始化内存池（必须在程序启动时调用）
void initializeUserPool() {
    PooledObject::initializePool<User>(100); // 预分配100个User对象
}
