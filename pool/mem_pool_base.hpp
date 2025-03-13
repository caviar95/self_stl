#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>
#include <mutex>

// 内存池基类
class MemoryPoolBase {
protected:
    // 内存块结构（嵌入式指针优化）
    struct Block {
        Block* next;
    };

    std::vector<Block*> blocks_; // 所有内存块（用于析构时释放）
    Block* free_list_ = nullptr; // 空闲链表头
    size_t block_size_;          // 每个块的大小
    size_t chunk_size_;          // 每次扩展的块数量

    // 扩展内存池容量
    void expand(size_t num_blocks) {
        if (num_blocks == 0) num_blocks = 1;

        // 分配新的内存块
        Block* new_block = static_cast<Block*>(std::malloc(num_blocks * block_size_));
        if (!new_block) throw std::bad_alloc();
        blocks_.push_back(new_block);

        // 初始化空闲链表
        for (size_t i = 0; i < num_blocks; ++i) {
            Block* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(new_block) + i * block_size_);
            block->next = free_list_;
            free_list_ = block;
        }
    }

    // 构造函数（仅允许派生类调用）
    MemoryPoolBase(size_t block_size, size_t chunk_size = 64)
        : block_size_(block_size), chunk_size_(chunk_size) {
        expand(chunk_size_);
    }

public:
    virtual ~MemoryPoolBase() {
        for (auto block : blocks_) {
            std::free(block);
        }
    }

    // 分配内存
    void* allocate() {
        if (!free_list_) {
            expand(chunk_size_); // 动态扩容
        }

        Block* block = free_list_;
        free_list_ = free_list_->next;
        return static_cast<void*>(block);
    }

    // 释放内存
    void deallocate(void* p) {
        if (!p) return;

        Block* block = static_cast<Block*>(p);
        block->next = free_list_;
        free_list_ = block;
    }
};
