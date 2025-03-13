#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <iostream>
#include <mutex>

template <typename T>
class MemoryPool {
private:
    struct Block {
        Block* next;
        alignas(T) unsigned char data[sizeof(T)];
    };

    std::vector<Block*> blocks_;
    Block* free_list_ = nullptr;
    size_t chunk_size_;

    void expand(size_t num_blocks) {
        if (num_blocks == 0) num_blocks = 1;
        Block* new_block = static_cast<Block*>(std::malloc(num_blocks * sizeof(Block)));
        blocks_.push_back(new_block);

        for (size_t i = 0; i < num_blocks; ++i) {
            Block* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(new_block) + i * sizeof(Block));
            block->next = free_list_;
            free_list_ = block;
        }
    }

public:
    MemoryPool(size_t chunk_size = 64) : chunk_size_(chunk_size) {
        expand(chunk_size_);
    }

    ~MemoryPool() {
        for (auto block : blocks_) std::free(block);
    }

    // 返回 unique_ptr 并绑定自定义删除器
    template <typename... Args>
    std::unique_ptr<T, std::function<void(T*)>> create(Args&&... args) {
        if (!free_list_) expand(chunk_size_);
        Block* block = free_list_;
        free_list_ = free_list_->next;

        T* obj = new (&block->data) T(std::forward<Args>(args)...); // 原地构造
        return std::unique_ptr<T, std::function<void(T*)>>(
            obj,
            [this](T* p) { // 自定义删除器：析构对象并归还内存
                p->~T();
                Block* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(p) - offsetof(Block, data));
                block->next = free_list_;
                free_list_ = block;
            }
        );
    }
};
