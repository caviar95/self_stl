#include "rbtree_self.hpp"

int main() {
    RBTree tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(15);
    tree.insert(25);

    tree.inorder();

    return 0;
}
