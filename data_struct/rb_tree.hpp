#pragma once

#include <iostream> // 测试用

enum Color { RED, BLACK };

template <typename T>
struct RBTreeNode {
    T key;
    Color color;
    RBTreeNode *left;
    RBTreeNode *right;
    RBTreeNode *parent;

    /*
        // 为什么默认红色？
        1 插入操作的简化：
        在红黑树中，插入新节点时，通常将新节点着色为红色。这是因为红色节点不违反任何红黑树的性质（除了可能出现两个连续的红色节点的情况），
        这样可以在插入后进行局部调整（如旋转和重新着色）来恢复树的平衡，而无需进行全局调整。
        如果新节点默认为黑色，则可能立即违反红黑树的性质4（从任一节点到其每个叶子的所有路径都包含相同数目的黑色节点），这将需要更复杂的调整来保持树的平衡。
        
        2 调整策略的便利：
        当插入红色节点导致出现两个连续的红色节点时，可以通过旋转和重新着色来局部修正这个问题，而保持其他路径的性质不变。
        相比之下，如果插入黑色节点，调整过程可能更加复杂，且可能影响到更多路径，从而增加维护树平衡性的难度。
        实现上的惯例：

        在多数红黑树的实现中，新插入的节点通常被设为红色，这是一种广泛采用的惯例，有助于简化插入和调整的逻辑。
     */
    RBTreeNode(T key) : key(key), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

template <typename T>
class RBTree {
public:
    RBTree() {
        InitNil();
    }

    /*
        查找节点
     */
    RBTreeNode<T> *Search(T key) {
        RBTreeNode<T> *x = root;
        while (x != nil) {
            if (key == x->key) {
                return x;
            } else if (key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        return nullptr;
    }

    /*
        插入操作
     */
    void Insert(T key) {
        RBTreeNode<T> *z = new RBTreeNode<T>(key);
        RBTreeNode<T> *y = nil;
        RBTreeNode<T> *x = root;

        while (x != nil) {
            y = x;
            if (z->key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        z->parent = y;
        if (y == nil) {
            root = z;
        } else if (z->key < y->key) {
            y->left = z;
        } else {
            y->right = z;
        }

        z->left = nil;
        z->right = nil;

        InsertFixup(z);
    }

    /*
        删除操作
     */
    void Remove(T key) {
        RBTreeNode<T> *z = Search(key);
        if (z == nullptr) {
            return;
        }

        RBTreeNode<T> *y = z;
        RBTreeNode<T> *x;
        Color y_original_color = y->color;

        if (z->left == nil) {
            x = z->right;
            Transplant(z, z->right);
        } else if (z->right == nil) {
            x = z->left;
            Transplant(z, z->left);
        } else {
            y = Minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                Transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            Transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;
        if (y_original_color == BLACK) {
            DeleteFixup(x);
        }
    }

    // 测试用
    void InOrder(RBTreeNode<T> *x) {
        if (x != nil) {
            InOrder(x->left);
            std::cout << x->key << "(" << (x->color == RED ? "RED" : "BLACK") << ") ";
            InOrder(x->right);
        }
    }

    void Print() {
        InOrder(root);
        std::cout << std::endl;
    }

private:
    RBTreeNode<T> *root;
    RBTreeNode<T> *nil; // 哨兵节点

    // 初始化哨兵节点
    void InitNil() {
        nil = new RBTreeNode<T>(T());
        nil->color = BLACK;
        root = nil;
    }

    /*
        左旋操作的核心是将x的右子节点y提升为新的父节点，并将原来y的左子树挂到x上。
        右子节点变为x的父节点，原来右子节点的左子树变为x的右子树
     */
    void LeftRotate(RBTreeNode<T> *x) {
        RBTreeNode<T> *y = x->right; // 取出x的右子节点，左旋的核心节点
        x->right = y->left;
        
        // 更新y原左子节点的父节点为x
        if (y->left != nil) {
            y->left->parent = x;
        }

        // 更新y的父节点为x的父节点，更新父子关系
        y->parent = x->parent;

        if (x->parent == nil) { // 如果原来x是根节点，更新根节点为y
            root = y;
        } else if (x == x->parent->left) { // 如果x是其父节点的左子节点，更新原父节点与y的关系
            x->parent->left = y;
        } else { // 如果x是其父节点的右子节点，更新原父节点与y的关系
            x->parent->right = y;
        }

        // 更新y与x的父子关系
        y->left = x;
        x->parent = y;
    }

    /*
        右旋操作的核心是将y的左子节点x提升为新的父节点，并将原来x的右子树挂到y上。
     */
    void RightRotate(RBTreeNode<T> *y) {
        RBTreeNode<T> *x = y->left;

        // 更新x原右子节点的父节点为y
        y->left = x->right;
        if (x->right != nil) {
            x->right->parent = y;
        }

        // 更新x的父节点为y的父节点，更新父子关系
        x->parent = y->parent;

        if (y->parent == nil) { // 如果原来y是根节点，更新根节点为x
            root = x;
        } else if (y == y->parent->left) { // 如果原来y是其父节点的左子节点，更新原父节点与x的关系
            y->parent->left = x;
        } else { // 如果原来y是其父节点的右子节点，更新原父节点与x的关系
            y->parent->right = x;
        }

        // 更新x与y的父子关系
        x->right = y;
        y->parent = x;
    }

    /*
        插入修复操作，确保红黑树性质
     */
    void InsertFixup(RBTreeNode<T> *z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) { // 如果其中一个是nil怎么办？
                RBTreeNode<T> *y = z->parent->parent->right;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        LeftRotate(z);
                    }

                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    RightRotate(z->parent->parent);
                }
            } else {
                RBTreeNode<T> *y = z->parent->parent->left;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        RightRotate(z);
                    }

                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    LeftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    /*
        删除修复操作，确保红黑树性质
     */
    void DeleteFixup(RBTreeNode<T> *x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                RBTreeNode<T> *w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    LeftRotate(x->parent);
                    w = x->parent->right;
                }

                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        RightRotate(w);
                        w = x->parent->right;
                    }

                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    LeftRotate(x->parent);
                    x = root;
                }
            } else {
                RBTreeNode<T> *w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    RightRotate(x->parent);
                    w = x->parent->left;
                }

                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        LeftRotate(w);
                        w = x->parent->left;
                    }

                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    RightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    /*
        节点移植，用于删除操作
     */
    void Transplant(RBTreeNode<T> *u, RBTreeNode<T> *v) {
        if (u->parent == nil) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    /*
        查找最小节点
     */
    RBTreeNode<T> *Minimum(RBTreeNode<T> *x) {
        while (x->left != nil) {
            x = x->left;
        }
        return x;
    }

    /*
        递归删除节点
     */
    void Destroy(RBTreeNode<T> *x) {
        if (x != nil) {
            Destroy(x->left);
            Destroy(x->right);
            delete x;
        }
    }
};
