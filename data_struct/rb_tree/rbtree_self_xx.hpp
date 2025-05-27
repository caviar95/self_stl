
void RotateLeft(Node* &root, Node *x) {
    // this oper processed with x->right != nullptr
    Node *y = x->right;

    x->right = y->left;

    // y->left & x
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

    y->parent = x->parent;

    y->left = x;

    x->parent = y;
}

void RotateRight(Node *x) {
    Node *y = x->left;

    x->left = y->right;

    if (y->right != nullptr) {
        y->right->parent = x;
    }

    y->parent = x->parent;

    if (nullptr == x->parent) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->right = x;
    x->parent = y;
}

/* incompleted version
void InsertFixup(Node *node) {
    while (node != root && node->parent->color == RED) {
        Node *gp = node->parent->parent;

        // if p is left node
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
                    RotateLeft(node);
                }

                node->parent->color = BLACK;
                gp->color = RED;
                RotateRight(gp);
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
                    RotateRight(node);
                }

                node->parent->color = BLACK;
                gp->color = RED;
                RotateLeft(gp);
            }
        }

        root->color = BLACK;
    }
    */

    /*
    void InsertFixup(Node *node) {
        while (node != root && node->parent != nullptr && node->parent->color == RED) {
            Node *gp = node->parent->parent;

            if (gp == nullptr) {
                break;
            }

            // if parent is left node
            if (node->parent == gp->left) {
               Node *uncle = gp->right;
               // if uncle is red, set parent and uncle as black, because cur is red
               if (uncle != nullptr && uncle->color == RED) {
                   node->parent->color = BLACK;
                   uncle->color = BLACK;
                   node = gp;
               } else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        RotateLeft(node);
                    }

                    node->parent->color = BLACK;
                    gp->color = RED;
                    node = gp;

                    RotateRight(gp);
               }
            } else {
                Node *uncle = gp->left;

                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        RotateRight(node);
                    }

                    node->parent->color = BLACK;
                    gp->color = RED;
                    RotateLeft(gp);
                }
            }
        }

        root->color = BLACK;
    }
    */
    /*
    void InsertFixup(Node *node) {
        while (node != root && node->parent != nullptr && node->parent->color == RED) {
            Node *pa = node->parent;
            Node *gp = pa->parent;

            if (gp == nullptr) {
                break;
            }

            if (pa == gp->left) {
                Node *uncle = gp->right;

                if (uncle != nullptr && uncle->color == RED) {
                    pa->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == pa->right) {
                        node = pa;
                        RotateLeft(node);
                    }

                    pa = node->parent;
                    gp = pa->parent;

                    RotateRight(gp);
                    pa->color = BLACK;
                    gp->color = RED;
                    node = pa;
                }
            } else {
                Node *uncle = gp->left;

                if (uncle != nullptr && uncle->color == RED) {
                    pa->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == pa->left) {
                        node = pa;
                        RotateRight(node);
                    }

                    pa = node->parent;
                    gp = pa->parent;

                    RotateLeft(gp);
                    pa->color = BLACK;
                    gp->color = RED;
                    node = pa;
                }
            }
        }

        root->color = BLACK;
    }
    */

    void InsertFixup(Node *node) {
        while (node != root && node->parent != nullptr && node->parent->color == RED) {
            Node *pa = node->parent;
            Node *gp = pa->parent;

            if (gp == nullptr) {
                break;
            }

            if (pa == gp->left) {
                Node *uncle = gp->right; // pa is left, then uncle is right

                if (uncle != nullptr && uncle->color == RED) {
                    pa->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == pa->right) {
                        node = pa;
                        RotateLeft(node);
                    }

                    node->parent->color = BLACK;
                    gp->color = RED;

                    RotateRight(gp);
                    node = node->parent;
                }
            } else {
                Node *uncle = gp->left;

                if (uncle != nullptr && uncle->color == RED) {
                    pa->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    node = gp;
                } else {
                    if (node == pa->left) {
                        node = pa;
                        RotateRight(node);
                    }

                    node->parent->color = BLACK;
                    gp->color = RED;
                    RotateLeft(gp);
                    node = node->parent;
                }
            }
        }

        root->color = BLACK;
    }
}
