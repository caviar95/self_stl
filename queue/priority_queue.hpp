#pragma once

// 前置
#include <vector>
#include <algorithm>
#include <functional>
#include <stdexcept>

// 优先级队列模板类，默认使用小顶堆逻辑
template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
private:
    std::vector<T> heap;
    Compare comp;
    
    
    // 上浮逻辑
    void HeapifyUp(int index)
    {
        while (index > 0) {
            int parent = (index - 1) / 2;
            // 下沉逻辑，如果父节点小于子节点，则交换位置
            if (comp(heap[parent], heap[index])) {
                std::swap(heap[parent], heap[index]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // 下沉逻辑
    // 如果当前节点比左子节点小，那么左子节点可能是更大的，同理右子节点
    // 如果largest不等于当前节点，说明需要交换，并递归
    void HeapifyDown(int index)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;
        if (left < heap.size() && comp(heap[largest], heap[left])) {
            largest = left;
        }

        if (right < heap.size() && comp(heap[largest], heap[right])) {
            largest = right;
        }

        if (largest != index) {
            std::swap(heap[largest], heap[index]);
            HeapifyDown(largest);
        }
    }

public:
    PriorityQueue() = default;
    explicit PriorityQueue(const Compare& c) : comp(c) {}

    void Push(const T& value)
    {
        heap.push_back(value);
        HeapifyUp(heap.size() - 1);
    }

    void Pop() {
        if (Empty()) {
            throw std::runtime_error("Heap is empty");
        }

        heap[0] = heap.back();
        heap.pop_back();
        
        // 当堆只有一个元素时，不需要下沉，否则会访问无效索引
        if (!Empty()) {
            HeapifyDown(0);
        }
    }

    const T& Top() const {
        if (Empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return heap[0];
    }

    bool Empty() const {
        return heap.empty();
    }

    size_t size() const {
        return heap.size();
    }
};

