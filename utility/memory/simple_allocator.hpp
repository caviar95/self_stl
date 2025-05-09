#pragma once

#include <memory>
#include <iostream>
#include <atomic>
#include <mutex>

/*
    Allocation/Deallocation:

        allocate() uses ::operator new to allocate raw memory

        deallocate() uses ::operator delete to free memory

        Both track and report their actions via std::cout

    Object Lifecycle Management:

        construct() uses placement new to initialize objects

        destroy() explicitly calls the destructor

        Both include logging of their operations

    Standard Requirements:

        Defines value_type as required for allocators

        Implements rebind mechanism to allow container internals to allocate related types (like list nodes)
*/

// template <typename T>
// class SimpleAllocator {
// public:
//     using value_type = T;

//     inline static std::atomic<size_t> alloc_count = 0;
//     inline static std::atomic<size_t> dealloc_count = 0;

//     static std::mutex alloc_mutex;

//     // 默认构造函数
//     SimpleAllocator() noexcept = default;

//     // 关键修复：跨类型转换构造函数
//     template <typename U>
//     SimpleAllocator(const SimpleAllocator<U>&) noexcept {}

//     // 分配内存
//     T* allocate(size_t n) {
//         std::lock_guard<std::mutex> lock(alloc_mutex);
//         alloc_count += n;
//         return static_cast<T*>(::operator new(n * sizeof(T)));
//     }

//     // 释放内存
//     void deallocate(T* p, size_t n) noexcept {
//         std::lock_guard<std::mutex> lock(alloc_mutex);
//         dealloc_count += n;
//         ::operator delete(p);
//     }

//     // 构造对象
//     template <typename U, typename... Args>
//     void construct(U* p, Args&&... args) {
//         new (p) U(std::forward<Args>(args)...);
//     }

//     // 销毁对象
//     template <typename U>
//     void destroy(U* p) noexcept {
//         p->~U();
//     }

//     // rebind 机制
//     template <typename U>
//     struct rebind {
//         using other = SimpleAllocator<U>;
//     };

//     // 关键修复：跨类型相等性比较
//     template <typename U>
//     bool operator==(const SimpleAllocator<U>&) const noexcept { 
//         return true; 
//     }

//     template <typename U>
//     bool operator!=(const SimpleAllocator<U>&) const noexcept { 
//         return false; 
//     }
// };

// template <typename T>
// std::mutex SimpleAllocator<T>::alloc_mutex;


template <typename T>
class SimpleAllocator {
public:
    using value_type = T;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    SimpleAllocator() noexcept = default;

    template <typename U>
    SimpleAllocator(const SimpleAllocator<U>&) noexcept {}

    T* allocate(size_t n) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (n > std::numeric_limits<size_t>::max() / sizeof(T)) {
            throw std::bad_array_new_length();
        }
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, size_t n) noexcept {
        std::lock_guard<std::mutex> lock(mtx_);
        ::operator delete(p);
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) noexcept {
        p->~U();
    }

    template <typename U>
    struct rebind {
        using other = SimpleAllocator<U>;
    };

    template <typename U>
    bool operator==(const SimpleAllocator<U>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const SimpleAllocator<U>&) const noexcept { return false; }

private:
    static inline std::mutex mtx_;
};
