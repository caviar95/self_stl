#pragma once

#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <mutex>

struct AllocationInfo {
    size_t size;
    const char *file;
    int line;
};

class MemoryTracker {
private:
    static std::unordered_map<void*, AllocationInfo> allocations_;
    static std::mutex mutex_;

public:
    static void Add(void *ptr, size_t size, const char *file, int line) {
        std::lock_guard<std::mutex> lk(mutex_);
        allocations_[ptr] = {size, file, line};
    }

    static void Remove(void *ptr) {
        std::lock_guard<std::mutex> lk(mutex_);
        auto it = allocations_.find(ptr);
        if (it == allocations_.end()) {
            allocations_.erase(it);
        } else {
            std::cerr << "double free or invalid at address: " << ptr << std::endl;
        }
    }

        static void Report() {
            
        }
};