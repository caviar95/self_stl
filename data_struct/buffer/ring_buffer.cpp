#include <atomic>
#include <memory>
#include <algorithm>
#include <thread>
#include <iostream>

template<typename T, size_t Capacity>
class RingBuffer {
public:
    RingBuffer() : buffer(std::make_unique<T[]>(Capacity)) {
        static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    }

    bool push(const T& item) {
        size_t current_write = write_ptr.load(std::memory_order_relaxed);
        size_t next_write = (current_write + 1) & (Capacity - 1);
        if (next_write == read_ptr.load(std::memory_order_acquire)) return false; // 满
        buffer[current_write] = item;
        write_ptr.store(next_write, std::memory_order_release);
        return true;
    }

    bool pop(T& item) {
        size_t current_read = read_ptr.load(std::memory_order_relaxed);
        if (current_read == write_ptr.load(std::memory_order_acquire)) return false; // 空
        item = buffer[current_read];
        read_ptr.store((current_read + 1) & (Capacity - 1), std::memory_order_release);
        return true;
    }

    size_t push_bulk(const T* data, size_t count) {
        size_t current_write = write_ptr.load(std::memory_order_relaxed);
        size_t current_read = read_ptr.load(std::memory_order_acquire);
        size_t used = current_write - current_read;
        size_t available = Capacity - used - 1; // 保留一个空位
        size_t to_write = std::min(count, available);
        for (size_t i = 0; i < to_write; ++i) {
            buffer[(current_write + i) & (Capacity - 1)] = data[i];
        }
        write_ptr.store((current_write + to_write) & (Capacity - 1), std::memory_order_release);
        return to_write;
    }

    size_t pop_bulk(T* data, size_t count) {
        size_t current_read = read_ptr.load(std::memory_order_relaxed);
        size_t current_write = write_ptr.load(std::memory_order_acquire);
        size_t available = current_write - current_read;
        size_t to_read = std::min(count, available);
        for (size_t i = 0; i < to_read; ++i) {
            data[i] = buffer[(current_read + i) & (Capacity - 1)];
        }
        read_ptr.store((current_read + to_read) & (Capacity - 1), std::memory_order_release);
        return to_read;
    }

private:
    std::unique_ptr<T[]> buffer;
    alignas(64) std::atomic<size_t> read_ptr{0};
    alignas(64) std::atomic<size_t> write_ptr{0};
};


int main() {
    RingBuffer<int, 8> rb;

    // 生产者线程
    std::thread producer([&rb]{
        for (int i = 0; i < 10; ++i) {
            while (!rb.push(i)) {} // 忙等待直到成功
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    // 消费者线程
    std::thread consumer([&rb]{
        int val;
        for (int i = 0; i < 10; ++i) {
            while (!rb.pop(val)) {}
            std::cout << "Consumed: " << val << std::endl;
        }
    });

    producer.join();
    consumer.join();
    return 0;
}