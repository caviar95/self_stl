#include "rb_tree.hpp"

#include "gtest/gtest.h"

TEST(RBTree, Test1) {
    RBTree<int> tree;

    tree.Insert(10);
    tree.Insert(20);
    tree.Insert(5);
    tree.Insert(15);
    tree.Insert(25);
    tree.Insert(3);
    tree.Insert(7);

    std::cout << "initial tree: ";
    tree.Print();  // 3(B) 5(R) 7(B) 10(B) 15(R) 20(B) 25(R)

    tree.Remove(10);
    std::cout << "after remove 10: ";
    tree.Print();  // 3(B) 5(R) 7(B) 15(B) 20(B) 25(R)

    tree.Remove(7);
    std::cout << "after remove 7: ";
    tree.Print();  // 3(B) 5(R) 15(B) 20(B) 25(R)
}
