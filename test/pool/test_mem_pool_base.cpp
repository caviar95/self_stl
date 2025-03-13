// #include "mem_pool_user.hpp"

#include "gtest/gtest.h"

// TEST(MemPoolBase, test1)
// {
//     // 创建对象（通过内存池分配）
//     User* user1 = new User(1, "Alice");
//     User* user2 = new User(2, "Bob");
//     user1->print();
//     user2->print();

//     // 销毁对象（内存返回池中）
//     delete user1;
//     delete user2;

//     // 复用内存创建新对象
//     User* user3 = new User(3, "Charlie");
//     user3->print();
//     delete user3;

// }


#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>
#include <mutex>

// 基类模板（通过 CRTP 为派生类提供内存池）
template <typename Derived>
class PooledObject {
private:
    // 内存池结构（每个派生类独立）
    struct MemoryPool {
        struct Block {
            Block* next;
            alignas(Derived) unsigned char data[sizeof(Derived)];
        };

        std::vector<Block*> blocks;
        Block* free_list = nullptr;
        size_t chunk_size;

        MemoryPool(size_t chunk) : chunk_size(chunk) {
            expand(chunk_size);
        }

        void expand(size_t num_blocks) {
            if (num_blocks == 0) num_blocks = 1;
            Block* new_block = static_cast<Block*>(std::malloc(num_blocks * sizeof(Block)));
            blocks.push_back(new_block);

            for (size_t i = 0; i < num_blocks; ++i) {
                Block* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(new_block) + i * sizeof(Block));
                block->next = free_list;
                free_list = block;
            }
        }

        void* allocate() {
            if (!free_list) expand(chunk_size);
            Block* block = free_list;
            free_list = free_list->next;
            return &block->data;
        }

        void deallocate(void* p) {
            Block* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(p) - offsetof(Block, data));
            block->next = free_list;
            free_list = block;
        }

        ~MemoryPool() {
            for (auto block : blocks) std::free(block);
        }
    };

    static MemoryPool pool; // 每个派生类拥有独立内存池

public:
    // 重载 operator new
    static void* operator new(size_t size) {
        if (size != sizeof(Derived)) return ::operator new(size);
        return pool.allocate();
    }

    // 重载 operator delete
    static void operator delete(void* p) {
        if (!p) return;
        pool.deallocate(p);
    }

    virtual ~PooledObject() = default;
};

// 静态成员初始化（必须放在头文件外）
template <typename Derived>
typename PooledObject<Derived>::MemoryPool PooledObject<Derived>::pool(64);

// 示例派生类
class User : public PooledObject<User> {
private:
    int id_;
    std::string name_;

public:
    User(int id, const std::string& name) : id_(id), name_(name) {
        std::cout << "User构造: ID=" << id_ << ", Name=" << name_ << "\n";
    }

    ~User() {
        std::cout << "User析构: ID=" << id_ << "\n";
    }

    void print() const {
        std::cout << "User ID: " << id_ << ", Name: " << name_ << "\n";
    }
};

TEST(MemPoolBase, test2)
{
    // 创建对象（通过内存池分配）
    User* user1 = new User(1, "Alice");
    User* user2 = new User(2, "Bob");
    user1->print();
    user2->print();

    // 销毁对象（内存返回池中）
    delete user1;
    delete user2;

    // 复用内存创建新对象
    User* user3 = new User(3, "Charlie");
    user3->print();
    delete user3;
}