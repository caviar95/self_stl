#pragma once

#include "mem_pool_base.hpp"

template <typename Derived>
class PooledObject {
private:
    static MemoryPool<Derived> pool; // 静态内存池

public:
    // 工厂方法：返回 unique_ptr
    template <typename... Args>
    static std::unique_ptr<Derived, std::function<void(Derived*)>> create(Args&&... args) {
        return pool.create(std::forward<Args>(args)...);
    }

    virtual ~PooledObject() = default;
};

// 静态成员初始化
template <typename Derived>
MemoryPool<Derived> PooledObject<Derived>::pool(64);
