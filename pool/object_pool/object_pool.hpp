#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

template <typename ObjectType, size_t PoolSize>
class ObjectPool {
    std::vector<std::unique_ptr<ObjectType>> pool;
    std::vector<bool> isObjectInUse;
    std::unordered_map<int, ObjectType*> idToObjectMap;

public:
    ObjectPool() {
        for (size_t i = 0; i < PoolSize; ++i) {
            pool.emplace_back(std::make_unique<ObjectType>());
            isObjectInUse.push_back(false);
            idToObjectMap[pool[i]->getId()] = pool[i].get();
        }
    }

    size_t Add(ObjectType t) {
        if (pool.size() < PoolSize) {
            pool.emplace_back(std::make_unique<ObjectType>(t));
            isObjectInUse.push_back(false);
            idToObjectMap[pool.back()->getId()] = pool.back().get();
            return pool.size() - 1;
        }
        return static_cast<size_t>(-1); // 表示添加失败
    }

    bool Del(size_t idx) {
        if (idx < pool.size() &&!isObjectInUse[idx]) {
            idToObjectMap.erase(pool[idx]->getId());
            pool.erase(pool.begin() + idx);
            isObjectInUse.erase(isObjectInUse.begin() + idx);
            return true;
        }
        return false;
    }

    ObjectType& Mod(ObjectType t) {
        auto it = idToObjectMap.find(t.getId());
        if (it != idToObjectMap.end()) {
            *(it->second) = t;
            return *(it->second);
        }
        return *Add(t); // 如果未找到则添加
    }

    const ObjectType& Get(size_t idx) const {
        if (idx < pool.size()) {
            return *pool[idx];
        }
        static ObjectType dummy;
        return dummy; // 返回一个空对象，可根据实际情况修改
    }
};
    