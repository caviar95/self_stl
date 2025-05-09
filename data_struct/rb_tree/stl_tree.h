#pragma once

namespace self_st {

enum rb_tree_color : bool {RED = false, BLACK = true};

struct rb_tree_node_base {
    using base_ptr = rb_tree_node_base*;
    using const_base_ptr = const rb_tree_node_base*;

    rb_tree_color color_;
    base_ptr parent_;
    base_ptr left_;
    base_ptr right_;

    static base_ptr minimum(base_ptr x) noexcept {
        while (x->left_ != nullptr) {
            x = x->left;
        }

        return x;
    }

    static const_base_ptr minimum(const_base_ptr x) noexcept {
        while (x->left_ != nullptr) {
            x = x->left_;
        }

        return x;
    }

    static base_ptr max
};
}