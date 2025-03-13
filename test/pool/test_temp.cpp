#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>
#include <new>
#include <list>
#include <forward_list>
#include <chrono>

// ====================== 通用内存池分配器 ======================
template <typename T>
class DataStructPoolAllocator {
private:
    // 内存块结构（嵌入式指针优化）
    struct Block {
        Block* next;
        alignas(T) unsigned char data[sizeof(T)]; // 内存对齐
    };

    std::vector<Block*> blocks_;   // 所有内存块（用于析构时释放）
    Block* free_list_ = nullptr;   // 空闲链表头
    size_t chunk_size_ = 64;       // 每次扩容的块数量

public:
    using value_type = T;          // 必须的类型定义
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    // 构造函数
    explicit DataStructPoolAllocator(size_t chunk_size = 64) 
        : chunk_size_(chunk_size) {
        expand(chunk_size_);
    }

    // 拷贝构造函数（支持容器拷贝）
    template <typename U>
    DataStructPoolAllocator(const DataStructPoolAllocator<U>& other) noexcept 
        : chunk_size_(other.chunk_size_) {}

    // 析构函数
    ~DataStructPoolAllocator() {
        for (auto block : blocks_) {
            ::operator delete(block);
        }
    }

    // 分配内存（符合Allocator接口）
    T* allocate(size_t n) {
        if (n != 1) { // 仅支持单对象分配（适用于链表节点等场景）
            throw std::bad_alloc();
        }

        if (!free_list_) {
            expand(chunk_size_); // 动态扩容
        }

        Block* block = free_list_;
        free_list_ = free_list_->next;
        return reinterpret_cast<T*>(&block->data);
    }

    // 释放内存（符合Allocator接口）
    void deallocate(T* p, size_t n) noexcept {
        if (!p || n != 1) return;

        Block* block = reinterpret_cast<Block*>(
            reinterpret_cast<char*>(p) - offsetof(Block, data)
        );
        block->next = free_list_;
        free_list_ = block;
    }

private:
    // 扩展内存池容量
    void expand(size_t num_blocks) {
        if (num_blocks == 0) num_blocks = 1;

        Block* new_block = static_cast<Block*>(
            ::operator new(num_blocks * sizeof(Block))
        );
        blocks_.push_back(new_block);

        for (size_t i = 0; i < num_blocks; ++i) {
            Block* block = &new_block[i];
            block->next = free_list_;
            free_list_ = block;
        }
    }
};

// 允许不同分配器实例间的比较
template <typename T, typename U>
bool operator==(const DataStructPoolAllocator<T>&, 
                const DataStructPoolAllocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const DataStructPoolAllocator<T>&, 
                const DataStructPoolAllocator<U>&) { return false; }

// ====================== 使用示例 ======================

// 示例1：自定义链表节点
template <typename T, typename Allocator = DataStructPoolAllocator<T>>
struct ListNode {
    T data;
    ListNode* next;

    // 使用分配器创建节点
    static ListNode* create(const T& val, Allocator& alloc) {
        ListNode* node = alloc.allocate(1);
        new (node) ListNode{val, nullptr};
        return node;
    }

    // 使用分配器销毁节点
    static void destroy(ListNode* node, Allocator& alloc) {
        node->~ListNode();
        alloc.deallocate(node, 1);
    }
};

// 示例2：STL容器集成
void test_stl_container() {
    // 使用内存池分配器的std::list
    using PooledList = std::list<int, DataStructPoolAllocator<int>>;
    DataStructPoolAllocator<int> alloc;
    PooledList lst(alloc);

    // 高频插入删除操作
    for (int i = 0; i < 100000; ++i) {
        lst.push_back(i);
        if (i % 2 == 0) {
            lst.pop_front();
        }
    }
}

// 示例3：树节点分配
template <typename T>
struct TreeNode {
    T data;
    TreeNode* left;
    TreeNode* right;

    // 使用分配器创建节点
    template <typename Allocator>
    static TreeNode* create(const T& val, Allocator& alloc) {
        TreeNode* node = alloc.allocate(1);
        new (node) TreeNode{val, nullptr, nullptr};
        return node;
    }

    // 使用分配器销毁节点（递归释放子树）
    template <typename Allocator>
    static void destroy(TreeNode* root, Allocator& alloc) {
        if (!root) return;
        destroy(root->left, alloc);
        destroy(root->right, alloc);
        root->~TreeNode();
        alloc.deallocate(root, 1);
    }
};

// 性能对比测试
void benchmark() {
    constexpr int COUNT = 1000000;

    // 标准分配器
    auto start1 = std::chrono::high_resolution_clock::now();
    {
        std::forward_list<int> lst;
        for (int i = 0; i < COUNT; ++i) {
            lst.push_front(i);
        }
    }
    auto duration1 = std::chrono::high_resolution_clock::now() - start1;

    // 内存池分配器
    auto start2 = std::chrono::high_resolution_clock::now();
    {
        std::forward_list<int, DataStructPoolAllocator<int>> lst;
        for (int i = 0; i < COUNT; ++i) {
            lst.push_front(i);
        }
    }
    auto duration2 = std::chrono::high_resolution_clock::now() - start2;

    std::cout << "std::allocator: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration1).count()
              << " ms\n";
    std::cout << "DataStructPoolAllocator: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration2).count()
              << " ms\n";
}

int main() {
    // 测试自定义链表
    DataStructPoolAllocator<ListNode<int>> list_alloc;
    ListNode<int>* node1 = nullptr;
    ListNode<int>* node2 = nullptr;

    try {
        node1 = ListNode<int>::create(1, list_alloc);
        node2 = ListNode<int>::create(2, list_alloc);
        node1->next = node2;
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        if (node1) ListNode<int>::destroy(node1, list_alloc);
        if (node2) ListNode<int>::destroy(node2, list_alloc);
        return 1;
    }

    ListNode<int>::destroy(node1, list_alloc);
    node1 = nullptr;
    ListNode<int>::destroy(node2, list_alloc);
    node2 = nullptr;

    // 测试STL容器
    test_stl_container();

    // 性能对比
    benchmark();

    return 0;
}
