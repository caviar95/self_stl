
enum Color {RED, BLACK};
template <typename T>
struct Node {
    T data;
    Color color;
    Node *left, *right, *parent;

    Node(T t) : data(t), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

template <typename T>
class RBTree {
private:
    using NodeType = Node<T>;    

    NodeType *root;
    NodeType *NIL;

    void RotateLeft(NodeType *x) {
        NodeType *y = x->right;
        x->right = y->left;

        if (y->left != NIL) {
            y->left->parent = x;
        }

        if (x->parent == NIL) {
            root = y;
        } else if (x->parent->left == x) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        y->parent = x->parent;
        x->parent = y;
        y->left = x;
    }

    void RotateRight(NodeType *x) {
        NodeType *y = x->left;
        x->left = y->right;

        if (y->right != NIL) {
            y->right->parent = x;
        }

        if (x->parent == NIL) {
            root = y;
        } else if (x->parent->left == x) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        y->parent = x->parent;
        x->parent = y;
        y->right = x;
    }

    void InsertFixup(NodeType *node) {
        while (node != root && node->parent != NIL && node->parent->color == RED) {
            NodeType *gp = node->parent->parent;
            NodeType *pa = node->parent;

            if (gp == NIL) {
                break;
            }

            // LL or LR
            if (pa == gp->left) {
                NodeType *uncle = gp->right;

                if (uncle != NIL && uncle->color == RED) {
                    pa->color = BLACK;
                    uncle->color = BLACK;
                    gp->color == RED;
                    node = gp;
                } else {
                    // LR
                    if (node == pa->right) {
                       node = pa;
                       RotateLeft(node);
                    }

                    // LL
                    pa->color = BLACK;
                    gp->color = RED;
                    RotateRight(gp);
                }
            } else {
                NodeType *uncle = gp->left;

                if (uncle != NIL && uncle->color == RED) {
                    pa->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    // RL
                    if (node == pa->left) {
                        node = pa;
                        RotateRight(node);
                    }

                    // RR
                    pa->color = BLACK;
                    gp->color = RED;
                    RotateLeft(gp);
                }
            }
        }

        root->color = BLACK;
    }

    void Transplant(NodeType *u, NodeType *v) {
        if (u->parent == NIL) {
            root = v;
            return;
        }

        if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }

        if (v != NIL) {
            v->parent = u->parent;
        }
    }

    NodeType* Minimium(NodeType* node) const {
        while (node != NIL) {
            node = node->left;
        }

        return node;
    }

    void DeleteFixup(NodeType* x) {
        if (x == NIL) {
            return;
        }

        NodeType *pa = x->parent;
        if (pa == NIL) {
            return;
        }

        while (x != root && x->color == BLACK) {
            if (x == pa->left) {
                NodeType *w = pa->right;

                if (w->color == RED) {
                    w->color = BLACK;
                    pa->color = RED;
                    RotateLeft(pa);
                    w = pa->right;
                }

                if (w->left != NIL && w->left->color == BLACK 
                        && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        RotateRight(w);
                        w = x->parent->right;
                    }

                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    RotateLeft(x->parent);
                    x = root;
                }
            } else {
                NodeType *w = x->parent->left;

                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    RotateRight(x->parent);
                    w = x->parent->left;
                }

                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        RotateLeft(w);
                        w = x->parent->left;
                    }

                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    RotateRight(x->parent);
                    x = root;
                }
            }
        }

        x->color = BLACK;

    }
public:
    RBTree() {
        NIL = new NodeType(T{}, BLACK);
        NIL->left = NIL->right = NIL->parent = NIL;
        root = NIL;
    }
};
