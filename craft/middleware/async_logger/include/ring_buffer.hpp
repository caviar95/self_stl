#pragma once

#include <atomic>
#include <array>

template<typename T, size_t N>
class MPSCRingBuffer {
public:
    // MPSCRingBuffer() : buffer_(new T[N]), capacity_(N) {
    //     head_.store(0, std::memory_order_relaxed);
    //     tail_.store(0, std::memory_order_relaxed);
    // }

    MPSCRingBuffer() : head_(0), tail_(0) {}

    ~MPSCRingBuffer() {
        // delete[] buffer_;
    }

    // buffer head for push, buffer tail for pop
    bool Push(const T& item) {
        // size_t head = head_.load(std::memory_order_relaxed);
        // size_t next = (head + 1) % capacity_;

        // if (next == tail_.load(std::memory_order_acquire)) {
        //     return false;  // 队列已满
        // }

        // buffer_[head] = item;
        // head_.store(next, std::memory_order_release);
        // return true;

        size_t head = head_.load(std::memory_order_relaxed);
        size_t next = (head + 1) % N;

        if (next == tail_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[head] = item;
        head_.store(next, std::memory_order_release);

        return true;
    }

    bool Pop(T& item) {
        size_t tail = tail_.load(std::memory_order_relaxed);

        if (tail == head_.load(std::memory_order_acquire)) {
            return false; // 队列为空
        }

        item = buffer_[tail];
        tail_.store((tail + 1) % N, std::memory_order_release);
        return true;
    }


private:
    std::array<T, N> buffer_;
    // const size_t capacity_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};
