#pragma once

#include <atomic>
#include <array>

// MPSC（多生产单消费）环形缓冲区模板

template<typename T, size_t N>
class MPSCRingBuffer {
public:
    bool push(const T& item) {
        size_t head, next;
        do {
            head = head_.load(std::memory_order_relaxed);
            next = (head + 1) % N;
            if (next == tail_.load(std::memory_order_acquire)) {
                return false; // 满了
            }
        } while (!head_.compare_exchange_weak(head, next, std::memory_order_acq_rel));

        buffer_[head] = item;
        return true;
    }

    bool pop(T& item) {
        size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) {
            return false; // 空
        }

        item = buffer_[tail];
        tail_.store((tail + 1) % N, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return head_.load() == tail_.load();
    }

private:
    std::array<T, N> buffer_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
};
