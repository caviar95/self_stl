#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>
#include <mutex>

// 模板化内存池（支持任意类型 T）
template <typename T>
class MemoryPool {
private:
    // 内存块结构（嵌入式指针优化）
    struct Block {
        Block* next;
        alignas(T) unsigned char data[sizeof(T)]; // 内存对齐
    };

    std::vector<Block*> blocks_;  // 所有内存块（用于析构时释放）
    Block* free_list_ = nullptr;  // 空闲链表头
    size_t chunk_size_;           // 每次扩展的块数量
    const size_t block_size_ = sizeof(Block); // 固定为 Block 结构大小

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

public:
    // 构造函数
    explicit MemoryPool(size_t chunk_size = 64) : chunk_size_(chunk_size) {
        expand(chunk_size_);
    }

    // 析构函数
    ~MemoryPool() {
        for (auto block : blocks_) {
            std::free(block);
        }
    }

    // 分配内存
    T* allocate() {
        if (!free_list_) {
            expand(chunk_size_); // 动态扩容
        }

        Block* block = free_list_;
        free_list_ = free_list_->next;
        return reinterpret_cast<T*>(&block->data); // 返回数据区地址
    }

    // 释放内存
    void deallocate(T* p) {
        if (!p) return;

        // 计算块地址（通过指针偏移）
        Block* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(p) - offsetof(Block, data));
        block->next = free_list_;
        free_list_ = block;
    }
};