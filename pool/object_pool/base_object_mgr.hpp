#pragma once


#include "object_pool.hpp"

template <typename ObjectType, size_t PoolSize>
class BaseObjectMgr {
protected:
    ObjectPool<ObjectType, PoolSize> pool;

    // 私有检查函数，派生类可重写
    virtual bool preAddCheck(const ObjectType& t) { return true; }
    virtual bool preDelCheck(size_t idx) { return true; }
    virtual bool preModCheck(const ObjectType& t) { return true; }
    virtual bool preGetCheck(size_t idx) { return true; }

public:
    size_t Add(ObjectType t) {
        if (preAddCheck(t)) {
            return pool.Add(t);
        }
        return static_cast<size_t>(-1);
    }

    bool Del(size_t idx) {
        if (preDelCheck(idx)) {
            return pool.Del(idx);
        }
        return false;
    }

    ObjectType& Mod(ObjectType t) {
        if (preModCheck(t)) {
            return pool.Mod(t);
        }
        static ObjectType dummy;
        return dummy;
    }

    const ObjectType& Get(size_t idx) const {
        if (preGetCheck(idx)) {
            return pool.Get(idx);
        }
        static ObjectType dummy;
        return dummy;
    }
};
    