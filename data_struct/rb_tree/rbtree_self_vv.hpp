#pragma once

#include <iostream>
#include <functional>

enum Color {RED, BLACK};

struct RBNode {
    int key;
    Color color;
    RBNode *left;
    RBNode *right;
    RBNode *parent;
};

class RBTree {
private:
    RBNode *root;
    RBNode *NIL;

    void LeftRotate(RBNode *x) {
        RBNode *y = x->right;
        x->right = y->left;

        if (x->right != NIL) {
            x->right->parent = x;
        }

        y->parent = x->parent;

        if (x->parent == NIL) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        x->parent = y;
        y->left = x;
    }

    void RightRotate(RBNode *y) {
        RBNode *x = y->left;
        y->left = x->right;

        if (y->left != NIL) {
            y->left->parent = y;
        }

        x->parent = y->parent;

        if (y->parent == NIL) {
            root = x;
        } else if (y->parent->left = y) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }

        y->parent = x;
        x->right = y;
    }

    void InsertFixup(RBNode *z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                RBNode *y = z->parent->parent->right;
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
                RBNode *y = z->parent->parent->left;
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

    void Transplant(RBNode *u, RBNode *v) {
        if (u->parent == NIL) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }

        v->parent = u->parent; // if v is nullptr or NIL
    }

    void DeleteFixup(RBNode *x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                RBNode *w = x->parent->right;

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
                RBNode *w = x->parent->left;
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

    RBNode* Minimum(RBNode *node) {
        while (node->left != NIL) {
            node = node->left;
        }

        return node;
    }

public:
    RBTree() {
        NIL = new RBNode{0, BLACK, nullptr, nullptr, nullptr};
        root = NIL;
    }

    ~RBTree() {
        std::function<void(RBNode *)> deleteNode = [&](RBNode *node) {
            if (node != NIL) {
                deleteNode(node->left);
                deleteNode(node->right);
                delete node;
            }
        };

        deleteNode(root);
        delete NIL;
    }

    void Insert(int key) {
        RBNode *z = new RBNode{key, RED, NIL, NIL};
        RBNode *y = NIL;
        RBNode *x = root;

        while (x != NIL) {
            y = x;
            if (z->key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        z->parent = y;
        if (y == NIL) {
            root = z;
        } else if (z->key < y->key) {
            y->left = z;
        } else {
            y->right = z;
        }

        InsertFixup(z);
    }

    void Remove(int key) {
        RBNode *z = root;
        while (z != NIL && z->key != key) {
            if (key < z->key) {
                z = z->left;
            } else {
                z = z->right;
            }
        }

        if (z == NIL) {
            return;
        }

        RBNode *y = z;
        RBNode *x;
        Color yOriColor = y->color;

        if (z->left == NIL) {
            x = z->right;
            Transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            Transplant(z, z->left);
        } else {
            y = Minimum(z->right);
            yOriColor = y->color;
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

        if (yOriColor == BLACK) {
            DeleteFixup(x);
        }
    }

    void Inorder() {
        InorderHelper(root);
        std::cout << std::endl;
    }

    void InorderHelper(RBNode* node) {
        if (node != NIL) {
            InorderHelper(node->left);
            std::cout << node->key << " (" << (node->color == RED ? "R)" : "B)") << " ";
            InorderHelper(node->right);
        }
    }
};
