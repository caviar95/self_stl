#include "rbtree_self_vv.hpp"

#include <vector>

int main()
{
    RBTree tree;
    std::vector<int> sets{10, 20, 30, 15, 5, 1};

    for (auto x : sets) {
        tree.Insert(x);
    }

    tree.Inorder();

    tree.Remove(10);

    tree.Inorder();

    return 0;
}
