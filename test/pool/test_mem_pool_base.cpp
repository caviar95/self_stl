#include "mem_pool_user.hpp"

#include "gtest/gtest.h"

TEST(MemPoolBase, test1)
{
        // 初始化内存池
    initializeUserPool();

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

    // 销毁内存池
    PooledObject::destroyPool();
}