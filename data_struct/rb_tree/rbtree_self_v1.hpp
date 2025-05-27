#pragma once

#include <iostream>
#include <algorithm>

enum Color {RED, BLACK};

struct Node {
    int data;
    Color color;
    Node *left, *right, *parent;

    Node(int data) : data(data), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree {
private:
    Node *root;

    void rotateLeft(Node* &x) {

        Node *y = x->right;
        x->right = y->left;
        if (y->left != nullptr) {
            y->left->parent = x;
        }
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

    void rotateRight(Node* &x) {
        Node *y = x->right;
        x->right = y->left;
        if (y->left != nullptr) {
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

    void fixInsert(Node* &k) {
        Node *parent = nullptr;
        Node *grandparent = nullptr;

        while (k != root && k->color == RED && k->parent->color == RED) {
            parent = k->parent;
            grandparent = parent->parent;
            if (parent == grandparent->left) {
                Node *uncle = grandparent->right;

                if (uncle && uncle->color == RED) {
                    // case 1 - change the color
                    grandparent->color = RED;
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    k = grandparent;
                } else {
                    if (k == parent->right) {
                        // case 2 - left rotate
                        rotateLeft(parent);
                        k = parent;
                        parent = k->parent;
                    }
                    // case 3 - right rotate
                    rotateRight(grandparent);
                    std::swap(parent->color, grandparent->color);
                    k = parent;
                }
            } else {
                // mirror of above
                Node *uncle = grandparent->left;

                if (uncle && uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    k = grandparent;
                } else {
                    if (k == parent->left) {
                        rotateRight(parent);
                        k = parent;
                        parent = k->parent;
                    }

                    rotateLeft(grandparent);
                    std::swap(parent->color, grandparent->color);
                    k = parent;
                }
            }
        }

        root->color = BLACK;
    }

public:
    RBTree() : root(nullptr) {}

    void insert(int data) {
        Node *node = new Node(data);
        Node *y = nullptr;
        Node *x = root;

        while (x != nullptr) {
            y = x;
            if (node->data < x->data) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        node->parent = y;
        if (y == nullptr) {
            root = node;
        } else if (node->data < y->data) {
            y->left = node;
        } else {
            y->right = node;
        }

        fixInsert(node);
    }

    void inorder() {
        inorderHelper(root);
        std::cout << std::endl;
    }

    void inorderHelper(Node *root) {
        if (root == nullptr) {
            return;
        }

        inorderHelper(root->left);
        std::cout << root->data << (root->color == RED ? "R " : "B ");
        inorderHelper(root->right);
    }
};



