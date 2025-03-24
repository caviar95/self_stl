#pragma once

#include "singleton.hpp"
#include "object_pool.hpp"
#include "type_b.h"

constexpr size_t MyClassPoolSize = 5;

class TypeBMgr : public Singleton<TypeBMgr> {
    ObjectPool<TypeB, MyClassPoolSize> pool;

public:
    std::unique_ptr<TypeB> getObject() {
        return pool.acquire();
    }

    void returnObject(std::unique_ptr<TypeB> obj) {
        pool.release(std::move(obj));
    }
};
