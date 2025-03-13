#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>
#include <new>

// 内存池分配器模板（符合C++ Allocator要求）
template <typename T>
class PoolAllocator {
private:
    // 内存块结构（嵌入式指针优化）
    struct Block {
        Block* next;
        alignas(T) unsigned char data[sizeof(T)]; // 内存对齐
    };

    std::vector<Block*> blocks_; // 所有分配的内存块
    Block* free_list_ = nullptr; // 空闲链表
    size_t block_size_ = 64;     // 每次扩展的块数量

public:
    using value_type = T; // 必须的类型定义

    // 构造函数：预分配内存
    explicit PoolAllocator(size_t chunk_size = 64) : block_size_(chunk_size) {
        expand_memory(block_size_);
    }

    // 析构函数：释放所有内存
    ~PoolAllocator() {
        for (auto block : blocks_) {
            ::operator delete(block);
        }
    }

    // 分配内存（符合Allocator接口）
    T* allocate(size_t n) {
        if (n != 1 || !free_list_) { // 仅支持单对象分配
            expand_memory(block_size_);
        }

        Block* block = free_list_;
        free_list_ = free_list_->next;
        return reinterpret_cast<T*>(&block->data);
    }

    // 释放内存（符合Allocator接口）
    void deallocate(T* p, size_t n) noexcept {
        if (!p || n != 1) return;

        Block* block = reinterpret_cast<Block*>(
            reinterpret_cast<char*>(p) - offsetof(Block, data)
        );
        block->next = free_list_;
        free_list_ = block;
    }

private:
    // 扩展内存池容量
    void expand_memory(size_t num_blocks) {
        Block* new_block = static_cast<Block*>(
            ::operator new(num_blocks * sizeof(Block))
        );
        blocks_.push_back(new_block);

        for (size_t i = 0; i < num_blocks; ++i) {
            Block* block = &new_block[i];
            block->next = free_list_;
            free_list_ = block;
        }
    }
};

// 允许不同模板实例间的转换（可选）
template <typename T, typename U>
bool operator==(const PoolAllocator<T>&, const PoolAllocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const PoolAllocator<T>&, const PoolAllocator<U>&) { return false; }
