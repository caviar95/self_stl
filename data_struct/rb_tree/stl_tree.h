#pragma once
#include <cstddef>

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
            x = x->left_;
        }

        return x;
    }

    static const_base_ptr minimum(const_base_ptr x) noexcept {
        while (x->left_ != nullptr) {
            x = x->left_;
        }

        return x;
    }

    static base_ptr maximum(base_ptr x) noexcept {
        while (x->right_ != nullptr) {
            x = x->right_;
        }

        return x;
    }

    static const_base_ptr maximum(const_base_ptr x) noexcept {
        while (x->right_ != nullptr) {
            x = x->right_;
        }

        return x;
    }

    template <typename KeyCompare>
    struct rb_tree_key_compare {
        KeyCompare key_compare;

        rb_tree_key_compare() noexcept(
            is_nothrow_default_constructible<KeyCompare>::value) : key_compare() {}

            rb_tree_key_compare(const KeyCompare &comp) : key_compare(comp) {}
        
        rb_tree_key_compare(const rb_tree_key_compare&) = default;

        rb_tree_key_compare(rb_tree_key_compare&& x) noexcept(
            is_nothrow_copy_constructible<KeyCompare>::value) : key_compare(x.key_compare) {}
    };

    struct rb_tree_header {
        rb_tree_node_base header;
        size_t node_count;
        
        rb_tree_header() noexcept {
            header.color = RED;
            reset();
        }

        rb_tree_header(rb_tree_header&& x) noexcept {
            if (x.header.parent != nullptr) {
                move_data(x);
            } else {
                header.color = RED;
                reset();
            }
        }

        void move_data(rb_tree_header& from) {
            header.color_ = from.header.color_;
            header.parent_ = from.header.parent_;
            header.left_ = from.header.left_;
            header.right_ = from.header.right_;
            header.parent->parent_ = &header_;
            node_count = from.node_count;

            from.reset();
        }

        void reset() {
            
        }
    };
    
    rb_tree_node_base() = default;
    
};
}