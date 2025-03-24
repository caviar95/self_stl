#pragma once

#include "singleton.hpp"
#include "object_pool.hpp"
#include "type_a.h"

constexpr size_t MyClassPoolSize = 3;

class TypeAMgr : public Singleton<TypeAMgr> {
    ObjectPool<TypeA, MyClassPoolSize> pool;

public:
    std::unique_ptr<TypeA> getObject() {
        return pool.acquire();
    }

    void returnObject(std::unique_ptr<TypeA> obj) {
        pool.release(std::move(obj));
    }
};
