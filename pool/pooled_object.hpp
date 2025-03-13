#pragma once

#include "mem_pool_base.hpp"

// 对象基类（集成内存池）
class PooledObject {
private:
    static MemoryPoolBase* pool_; // 静态内存池指针

public:
    // 重载 operator new
    static void* operator new(size_t size) {
        if (!pool_) {
            throw std::bad_alloc(); // 内存池未初始化
        }
        return pool_->allocate();
    }

    // 重载 operator delete
    static void operator delete(void* p) {
        if (!p) return;
        pool_->deallocate(p);
    }

    // 初始化内存池（必须在派生类中调用）
    template <typename T>
    static void initializePool(size_t chunk_size = 64) {
        if (pool_) return; // 避免重复初始化
        pool_ = new MemoryPoolBase(sizeof(T), chunk_size);
    }

    // 销毁内存池
    static void destroyPool() {
        delete pool_;
        pool_ = nullptr;
    }

    virtual ~PooledObject() = default; // 虚析构函数
};

// 静态成员初始化
MemoryPoolBase* PooledObject::pool_ = nullptr;
