#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <stack>

#ifndef OBJECT_POOL_MIN_MAX_SIZE

// 对象池模板类
template<typename T>
class ObjectPool {
public:
    // 创建对象池，初始容量为 initialSize
    ObjectPool(size_t initialSize) {
        for (size_t i = 0; i < initialSize; ++i) {
            availableObjects.push(std::make_unique<T>());
        }
    }

    // 从池中获取对象（若池为空，则新建对象）
    std::unique_ptr<T, std::function<void(T*)>> allocate() {
        if (availableObjects.empty()) {
            // 池为空时创建新对象
            return { new T(), [this](T* obj) { deallocate(obj); } };
        } else {
            // 从池中取出对象
            auto obj = std::move(availableObjects.top());
            availableObjects.pop();
            return { obj.release(), [this](T* obj) { deallocate(obj); } };
        }
    }

    // 归还对象到池中
    void deallocate(T* obj) {
        // 重置对象状态（可选）
        obj->reset(-1, 0.0f, 0.0f);  // 将ID设为-1，坐标归零
        availableObjects.push(std::unique_ptr<T>(obj));
    }

private:
    std::stack<std::unique_ptr<T>> availableObjects; // 可用对象栈
};

#else

template <typename T>
class ObjectPool {
public:
    ObjectPool(size_t minSize, size_t maxSize)
        : minSize_(minSize), maxSize_(maxSize), currentSize_(minSize) { // 显式初始化 currentSize_
        for (size_t i = 0; i < minSize_; ++i) {
            pool_.emplace_back(std::make_unique<T>(generateId()));
        }
    }

    std::unique_ptr<T, std::function<void(T*)>> acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_.empty()) {
            if (currentSize_ < maxSize_) {
                // 原子化操作：创建对象并递增计数
                auto newObj = std::make_unique<T>(generateId());
                auto* rawPtr = newObj.release();
                currentSize_++;
                return { rawPtr, [this](T* p) { release(p); } };
            } else {
                throw std::runtime_error("Pool exhausted.");
            }
        } else {
            auto obj = std::move(pool_.back());
            pool_.pop_back();
            return { obj.release(), [this](T* p) { release(p); } };
        }
    }

    void release(T* obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        obj->reset(generateId());
        pool_.emplace_back(obj);
    }

private:
    std::vector<std::unique_ptr<T>> pool_;
    std::mutex mutex_;
    size_t minSize_;
    size_t maxSize_;
    size_t currentSize_; // 记录已创建对象总数（含已借出和池中空闲的）

    std::string generateId() {
        return "Conn_" + std::to_string(rand() % 1000);
    }
};

#endif
