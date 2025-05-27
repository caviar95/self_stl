#pragma once

#include <iostream>

#include <climits>

namespace Self {

enum Color {RED, BLACK};

struct Node {
    int key;
    Color color;
    Node *left, *right, *parent;

    Node(int key = INT_MAX) : key(key), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree {
private:
    Node *root;

    void RotateLeft(Node* &root, Node *x) {
        Node *y = x->right;
        x->right = y->left;

        if (y->left != nullptr) {
            y->left->parent = x;            
        }

        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
    }

    void RotateRight(Node* &root, Node *x) {
        Node *y = x->left;
        x->left = y->right;

        if (y->right != nullptr) {
            y->right->parent = x;
        }

        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }

        y->right = x;
        x->parent = y;
    }

    void InsertFixup(Node* &root, Node *node) {
        while (node != root && node->parent->color == RED) {
            Node *gp = node->parent->parent;

            if (node->parent == gp->left) {
                Node *uncle = gp->right;
                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        RotateLeft(root, node);
                    }

                    node->parent->color = BLACK;
                    gp->color = RED;
                    RotateRight(root, gp);
                }
            } else {
                Node *uncle = gp->left;
                if (uncle->parent->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        RotateRight(root, node);
                    }

                    node->parent->color = BLACK;
                    gp->color = RED;
                    RotateLeft(root, gp);
                }
            }
        }

        root->color = BLACK;
    }

    void Transplant(Node* &root, Node *u, Node *v) {
        if (u->parent == nullptr) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }

        if (v != nullptr) {
            v->parent = u->parent;
        }
    }

    Node *Minimum(Node *node) const {
        while (node->left != nullptr) {
            node = node->left;
        }

        return node;
    }

    void DeleteFixup(Node* &root, Node *x, Node *xParent) {
        while ((x == nullptr || x->color == BLACK) && x != root) {
            if (x == xParent->left) {
                Node *w = xParent->right;
                if (w == nullptr) {
                    break;
                }

                if (w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    RotateLeft(root, xParent);
                    w = xParent->right;
                }

                if ((w->left == nullptr || w->left->color == BLACK)
                        && (w->right == nullptr || w->right->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = xParent->parent;
                } else {
                    if (w->right == nullptr || w->right->color == BLACK) {
                        if (w->left != nullptr) {
                            w->left->color = BLACK;
                        }

                        w->color = RED;
                        RotateRight(root, w);
                        w = xParent->right;
                    }

                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->right) {
                        w->right->color = BLACK;
                    }

                    RotateLeft(root, xParent);
                    x = root;
                }
            } else {
                Node *w = xParent->left;
                if (w == nullptr) {
                    break;
                }

                if (w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    RotateRight(root, xParent);
                    w = xParent->left;
                }

                if ((w->left == nullptr || w->left->color == BLACK) &&
                        (w->right == nullptr || w->right->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = xParent->parent;
                } else {
                    if (w->left == nullptr || w->left->color == BLACK) {
                        if (w->right != nullptr) {
                            w->right->color = BLACK;
                        }

                        w->color = RED;
                        RotateLeft(root, w);
                        w = xParent->left;
                    }

                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->left != nullptr) {
                        w->left->color = BLACK;
                    }

                    RotateRight(root, xParent);
                    x = root;
                }
            }
        }

        if (x != nullptr) {
            x->color = BLACK;
        }
    }

    Node *Search(Node *node, int key) const {
        if (node == nullptr || key == node->key) {
            return node;
        }

        return key < node->key ? Search(node->left, key) : Search(node->right, key);
    }

    void DeleteNode(Node* &root, int key) {
        Node *z = Search(root, key);

        if (z == nullptr) {
            return;
        }

        Node *y = z;
        Node *x = nullptr;
        Node *xParent = nullptr;
        Color yOri = y->color;

        if (z->left == nullptr) {
            x = z->right;
            xParent = z->parent;
            Transplant(root, z, z->right);
        } else if (z->right == nullptr) {
            x = z->left;
            xParent = z->parent;
            Transplant(root, z, z->left);
        } else {
            y = Minimum(z->right);
            yOri = y->color;
            x = y->right;
            if (y->parent == z) {
                xParent = y;
            } else {
                Transplant(root, y, y->right);
                y->right = z->right;
                if (y->right != nullptr) {
                    y->right->parent = y;
                }
                xParent = y->parent;
            }

            Transplant(root, z, y);
            y->left = z->left;
            if (y->left != nullptr) {
                y->left->parent = y;
            }

            y->color = z->color;
        }

        delete z;

        if (yOri == BLACK) {
            DeleteFixup(root, x, xParent);
        }
    }

    void InorderHelper(Node *root) const {
        if (root == nullptr) {
            return;
        }

        InorderHelper(root->left);
        std::cout << root->key << "(" << (root->color == RED ? "R" : "B") << ")";
        InorderHelper(root->right);
    }

public:
    RBTree() : root(nullptr) {}

    void Insert(int key) {
        Node *node = new Node(key);
        Node *y = nullptr;
        Node *x = root;

        while (x != nullptr) {
            y = x;
            if (node->key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        node->parent = y;
        if (y == nullptr) {
            root = node;
        } else if (node->key < y->key) {
            y->left = node;
        } else {
            y->right = node;
        }

        InsertFixup(root, node);
    }

    void Remove(int key) {
        DeleteNode(root, key);
    }

    Node *Find(int key) const {
        return Search(root, key);
    }

    int FindMin() const {
        Node *min = Minimum(root);
        return min != nullptr ? min->key : -1;
    }

    int FindMax() const {
        Node *max = root;
        while (max != nullptr && max->right != nullptr) {
            max = max->right;
        }

        return max != nullptr ? max->key : -1;
    }

    void Inorder() const {
        InorderHelper(root);
        std::cout << std::endl;
    }
};

}
