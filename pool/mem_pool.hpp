/*
    关键优化点
        优化方向	    实现方法
        内存对齐	    将块大小对齐到 sizeof(void*)（通常8字节），提高访问效率。
        连续内存布局	预分配连续内存块，减少缓存未命中（Cache Miss）。
        零碎片分配	    固定块大小，避免内存碎片化。
        高效链表管理	利用空闲块的头部直接存储链表指针，省去额外元数据开销。
        无系统调用	    仅在初始化和销毁时调用 malloc/free，运行时无系统调用。
    性能对比
        操作	    传统 new/delete	        内存池
        分配时间	    高（系统调用）	    极低（链表操作）
        内存碎片	    高	                无
        适用场景	    任意大小对象	    固定大小对象
        线程安全性	    依赖实现	        单线程优化


*/


#pragma once

#include <iostream>
#include <cstdlib>
#include <stdexcept>

// 内存池类：管理固定大小的内存块
class MemoryPool {
public:
    /**
     * @brief 构造函数
     * @param block_size 每个内存块的大小（单位：字节）
     * @param num_blocks 预分配的内存块数量
     */
    MemoryPool(size_t block_size, size_t num_blocks) 
        : block_size_(align(block_size)), 
          num_blocks_(num_blocks),
          free_list_(nullptr),
          buffer_(nullptr) {
        
        // 分配连续内存空间
        buffer_ = static_cast<char*>(std::malloc(block_size_ * num_blocks_));
        if (!buffer_) {
            throw std::bad_alloc();
        }

        // 初始化空闲链表：每个块头部存储下一个块的地址
        char* p = buffer_;
        for (size_t i = 0; i < num_blocks_ - 1; ++i) {
            *reinterpret_cast<void**>(p) = p + block_size_;
            p += block_size_;
        }
        *reinterpret_cast<void**>(p) = nullptr; // 最后一个块指向空
        free_list_ = buffer_;
    }

    ~MemoryPool() {
        std::free(buffer_);
    }

    // 分配一个内存块
    void* allocate() {
        if (!free_list_) {
            throw std::bad_alloc(); // 无可用块时抛出异常
        }

        // 取出空闲链表头部块
        void* block = free_list_;
        free_list_ = *reinterpret_cast<void**>(free_list_);
        return block;
    }

    // 释放内存块到池中
    void deallocate(void* block) {
        if (!block) return;

        // 将块重新链接到空闲链表头部
        *reinterpret_cast<void**>(block) = free_list_;
        free_list_ = block;
    }

private:
    // 内存对齐（例如对齐到 8 字节）
    static size_t align(size_t size) {
        const size_t alignment = sizeof(void*);
        return (size + alignment - 1) & ~(alignment - 1);
    }

    size_t block_size_;  // 对齐后的块大小
    size_t num_blocks_;  // 总块数
    void* free_list_;    // 空闲链表头指针
    char* buffer_;       // 预分配的内存缓冲区
};

/*

// 线程安全内存池
#include <mutex>
class ThreadSafeMemoryPool {
public:
    void* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.allocate();
    }
    void deallocate(void* p) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.deallocate(p);
    }
private:
    MemoryPool pool_;
    std::mutex mutex_;
};

// 动态扩容
void* allocate() {
    if (!free_list_) {
        // 动态分配新块并链接到空闲链表
        expandPool();
    }
    // ... 原有分配逻辑
}

// 内存泄漏检测
~MemoryPool() {
    if (free_list_ != buffer_ || 未全部释放) {
        std::cerr << "Memory leak detected!" << std::endl;
    }
    std::free(buffer_);
}

*/
