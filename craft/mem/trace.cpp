#include <iostream>
#include <unordered_map>
#include <mutex>
#include <cstdlib>
#include <cstdio>

struct AllocationInfo {
    size_t size;
    const char* file;
    int line;
};

// 内存分配跟踪器
class MemoryTracker {
private:
    static std::unordered_map<void*, AllocationInfo> allocations;
    static std::mutex mutex;

public:
    static void add(void* ptr, size_t size, const char* file, int line) {
        std::lock_guard<std::mutex> lock(mutex);
        allocations[ptr] = {size, file, line};
    }

    static void remove(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = allocations.find(ptr);
        if (it != allocations.end()) {
            allocations.erase(it);
        } else {
            std::cerr << "Double free or invalid free at address: " << ptr << std::endl;
        }
    }

    static void report() {
        std::lock_guard<std::mutex> lock(mutex);
        if (allocations.empty()) {
            std::cout << "No memory leaks detected." << std::endl;
        } else {
            std::cerr << "\nMemory leak detected! Allocation details:\n";
            for (const auto& [ptr, info] : allocations) {
                std::cerr << "Address: " << ptr 
                        << "\nSize: " << info.size 
                        << "\nLocation: " << info.file 
                        << ":" << info.line << "\n\n";
            }
        }
    }
};

// 初始化静态成员
std::unordered_map<void*, AllocationInfo> MemoryTracker::allocations;
std::mutex MemoryTracker::mutex;

// 重载全局new运算符
void* operator new(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr) {
        MemoryTracker::add(ptr, size, file, line);
    }
    return ptr;
}

void* operator new[](size_t size, const char* file, int line) {
    return operator new(size, file, line);
}

// 常规delete运算符
void operator delete(void* ptr) noexcept {
    MemoryTracker::remove(ptr);
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    operator delete(ptr);
}

// 带size参数的delete（C++14+）
void operator delete(void* ptr, size_t) noexcept {
    operator delete(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
    operator delete(ptr);
}

// 定义宏替换new运算符
#ifdef _DEBUG
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#endif

// 示例用法
int main() {
    int* leakInt = new int(42);       // 故意泄漏
    int* safeInt = new int(100);
    delete safeInt;

    // 在程序结束时报告内存泄漏
    MemoryTracker::report();
    return 0;
}