/*
关键优化点
优化方向	实现方法
内存预分配	一次性分配连续内存块，避免多次 malloc/free。
零系统调用	对象创建/销毁仅操作链表，无系统级内存分配。
placement new	在预分配内存上直接构造对象，避免默认 new 的额外开销。
显式析构管理	手动调用析构函数，避免 delete 自动释放内存，实现内存复用。
类型安全	模板化设计，确保内存块大小与类对象严格匹配。
5. 性能对比
操作	传统 new/delete	内存池
内存分配时间	高（系统调用 + 可能碎片化）	极低（链表操作）
对象构造开销	两次内存操作（分配 + 构造）	一次构造（内存已预分配）
适用场景	通用对象创建	高频固定大小对象（如游戏实体）

 */

#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <new>
#include <vector>

template <typename T>
class ObjectPool {
private:
    struct Node {
        Node* next;  // 嵌入式指针实现空闲链表
        char data[sizeof(T)];  // 实际存储对象的内存空间
    };

    Node* free_list_;        // 空闲链表头节点
    std::vector<Node*> blocks_; // 所有内存块（便于析构时统一释放）

public:
    explicit ObjectPool(size_t chunk_size = 64) {
        expand(chunk_size);  // 预分配第一个内存块
    }

    ~ObjectPool() {
        // 释放所有内存块
        for (auto block : blocks_) {
            ::operator delete(block);
        }
    }

    // 分配对象内存
    void* allocate() {
        if (!free_list_) {
            expand(blocks_.size() * 2);  // 按当前块数翻倍扩容
        }

        Node* node = free_list_;
        free_list_ = free_list_->next;
        return static_cast<void*>(&node->data);
    }

    // 回收对象内存
    void deallocate(void* p) {
        Node* node = reinterpret_cast<Node*>(
            static_cast<char*>(p) - offsetof(Node, data)
        );
        node->next = free_list_;
        free_list_ = node;
    }

private:
    // 扩展内存池容量
    void expand(size_t num_nodes) {
        if (num_nodes == 0) num_nodes = 1;

        // 分配内存块（按节点大小对齐）
        Node* new_block = static_cast<Node*>(
            ::operator new(num_nodes * sizeof(Node))
        );
        blocks_.push_back(new_block);

        // 初始化空闲链表
        for (size_t i = 0; i < num_nodes; ++i) {
            Node* node = &new_block[i];
            node->next = free_list_;
            free_list_ = node;
        }
    }
};

/*
动态扩容
template <typename T>
template <typename... Args>
T* ObjectPool<T>::create(Args&&... args) {
    if (!free_list_) {
        // 动态扩容：新增内存块并链接到空闲链表
        expandPool();
    }
    // ... 原有逻辑
}

template <typename T>
void ObjectPool<T>::expandPool() {
    // 分配新内存块并链接到空闲链表
    char* new_buffer = static_cast<char*>(aligned_alloc(alignof(T), block_size_ * num_blocks_));
    // ... 类似构造函数中的链表初始化
    num_blocks_ *= 2; // 容量翻倍（示例策略）
}

线程安全
#include <mutex>

template <typename T>
class ThreadSafeObjectPool {
public:
    template <typename... Args>
    T* create(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        // ... 原有逻辑
    }

    void destroy(T* obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        // ... 原有逻辑
    }

private:
    ObjectPool<T> pool_;
    std::mutex mutex_;
};

 */