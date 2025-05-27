#include <iostream>
using namespace std;

enum Color { RED, BLACK };

struct Node {
    int key;
    Color color;
    Node *left, *right, *parent;

    Node(int key) : key(key), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree {
private:
    Node* root;

    void rotateLeft(Node*& root, Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node*& root, Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void insertFixup(Node*& root, Node* node) {
        while (node != root && node->parent->color == RED) {
            Node* gp = node->parent->parent;
            if (node->parent == gp->left) {
                Node* uncle = gp->right;
                if (uncle && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        rotateLeft(root, node);
                    }
                    node->parent->color = BLACK;
                    gp->color = RED;
                    rotateRight(root, gp);
                }
            } else {
                Node* uncle = gp->left;
                if (uncle && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        rotateRight(root, node);
                    }
                    node->parent->color = BLACK;
                    gp->color = RED;
                    rotateLeft(root, gp);
                }
            }
        }
        root->color = BLACK;
    }

    void transplant(Node*& root, Node* u, Node* v) {
        if (!u->parent) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    Node* minimum(Node* node) {
        while (node->left) node = node->left;
        return node;
    }

    void deleteFixup(Node*& root, Node* x, Node* xParent) {
        while (x != root && (!x || x->color == BLACK)) {
            if (x == xParent->left) {
                Node* w = xParent->right;
                if (w && w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    rotateLeft(root, xParent);
                    w = xParent->right;
                }
                if ((!w->left || w->left->color == BLACK) &&
                    (!w->right || w->right->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (!w->right || w->right->color == BLACK) {
                        if (w->left) w->left->color = BLACK;
                        w->color = RED;
                        rotateRight(root, w);
                        w = xParent->right;
                    }
                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->right) w->right->color = BLACK;
                    rotateLeft(root, xParent);
                    x = root;
                }
            } else {
                Node* w = xParent->left;
                if (w && w->color == RED) {
                    w->color = BLACK;
                    xParent->color = RED;
                    rotateRight(root, xParent);
                    w = xParent->left;
                }
                if ((!w->right || w->right->color == BLACK) &&
                    (!w->left || w->left->color == BLACK)) {
                    w->color = RED;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if (!w->left || w->left->color == BLACK) {
                        if (w->right) w->right->color = BLACK;
                        w->color = RED;
                        rotateLeft(root, w);
                        w = xParent->left;
                    }
                    w->color = xParent->color;
                    xParent->color = BLACK;
                    if (w->left) w->left->color = BLACK;
                    rotateRight(root, xParent);
                    x = root;
                }
            }
        }
        if (x) x->color = BLACK;
    }

    Node* search(Node* node, int key) const {
        if (!node || key == node->key) return node;
        return key < node->key ? search(node->left, key) : search(node->right, key);
    }

    void deleteNode(Node*& root, int key) {
        Node* z = search(root, key);
        if (!z) return;

        Node* y = z;
        Node* x = nullptr;
        Node* xParent = nullptr;
        Color yOriginalColor = y->color;

        if (!z->left) {
            x = z->right;
            xParent = z->parent;
            transplant(root, z, z->right);
        } else if (!z->right) {
            x = z->left;
            xParent = z->parent;
            transplant(root, z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == z) xParent = y;
            else {
                transplant(root, y, y->right);
                y->right = z->right;
                if (y->right) y->right->parent = y;
            }
            transplant(root, z, y);
            y->left = z->left;
            if (y->left) y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (yOriginalColor == BLACK)
            deleteFixup(root, x, xParent);
    }

    void inorderHelper(Node* root) const {
        if (!root) return;
        inorderHelper(root->left);
        cout << root->key << "(" << (root->color == RED ? "R" : "B") << ") ";
        inorderHelper(root->right);
    }

public:
    RBTree() : root(nullptr) {}

    void insert(int key) {
        Node* node = new Node(key);
        Node* y = nullptr;
        Node* x = root;

        while (x) {
            y = x;
            if (node->key < x->key) x = x->left;
            else x = x->right;
        }

        node->parent = y;
        if (!y) root = node;
        else if (node->key < y->key) y->left = node;
        else y->right = node;

        insertFixup(root, node);
    }

    void remove(int key) {
        deleteNode(root, key);
    }

    Node* find(int key) const {
        return search(root, key);
    }

    int findMin() const {
        Node* min = minimum(root);
        return min ? min->key : -1;
    }

    int findMax() const {
        Node* max = root;
        while (max && max->right) max = max->right;
        return max ? max->key : -1;
    }

    void inorder() const {
        inorderHelper(root);
        cout << endl;
    }
};

int main() {
    RBTree tree;
    int values[] = {10, 5, 15, 2, 7, 20};
    for (int val : values) tree.insert(val);
    tree.inorder();

    cout << "Min: " << tree.findMin() << endl;
    cout << "Max: " << tree.findMax() << endl;

    tree.remove(5);
    tree.inorder();

    return 0;
}
