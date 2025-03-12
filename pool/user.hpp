#pragma once

#include "object_alloc_pool.hpp"

class User {
    int id;
    std::string name;

    static ObjectPool<User> pool;  // 静态内存池

public:
    // 构造函数
    User(int id, const std::string& name) : id(id), name(name) {}

    // 析构函数（保持空实现，实际由内存池管理）
    ~User() = default;

    // 打印用户信息
    void print() const {
        std::cout << "User ID: " << id 
                  << ", Name: " << name << std::endl;
    }

    // 重载 operator new/delete
    static void* operator new(size_t size) {
        std::cout << "user defined new" << std::endl;
        if (size != sizeof(User)) {
            return ::operator new(size);  // 非 User 类型走默认分配
        }
        return pool.allocate();
    }

    static void operator delete(void* p) {
        if (!p) return;
        pool.deallocate(p);
    }
};

// 初始化静态成员
ObjectPool<User> User::pool;