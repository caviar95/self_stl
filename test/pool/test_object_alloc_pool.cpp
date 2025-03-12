#include "user.hpp"

#include "gtest/gtest.h"

TEST(ObjectPoolWithAlloc, MyClass) {
    User* user1 = new User(1, "Alice");
    User* user2 = new User(2, "Bob");
    User* user3 = new User(3, "Charlie");

    // 使用对象
    user1->print();
    user2->print();
    user3->print();

    // 释放对象（内存返回池中）
    delete user1;
    delete user2;

    // 复用内存创建新对象
    User* user4 = new User(4, "David");
    User* user5 = new User(5, "Eve");

    user4->print();
    user5->print();

    // 释放所有对象
    delete user3;
    delete user4;
    delete user5;
}