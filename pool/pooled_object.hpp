#pragma once

#include "mem_pool_base.hpp"

// 对象基类（无需继承内存池）
class PooledObject {
private:
    template <typename T>
    static MemoryPool<T>* getPool() {
        static MemoryPool<T> pool(64); // 每个类型 T 有独立内存池
        return &pool;
    }

public:
    // 重载 operator new
    template <typename T>
    static void* operator new(size_t size) {
        if (size != sizeof(T)) {
            return ::operator new(size); // 非 T 类型走默认分配
        }
        return getPool<T>()->allocate();
    }

    // 重载 operator delete
    template <typename T>
    static void operator delete(void* p) {
        if (!p) return;
        getPool<T>()->deallocate(static_cast<T*>(p));
    }

    virtual ~PooledObject() = default; // 虚析构函数
};
