#include "rbtree_self.hpp"

int main()
{
    using namespace Self;

    RBTree tree;

    int values[] = {10, 5, 15, 2, 7, 20};

    for (int val : values) {
        tree.Insert(val);
    }

    tree.Inorder();

    std::cout << "Min: " << tree.FindMin() << std::endl;
    std::cout << "Max: " << tree.FindMax() << std::endl;

    tree.Remove(5);

    tree.Inorder();

    return 0;
}
