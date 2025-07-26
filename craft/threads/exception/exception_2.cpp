#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

std::atomic<bool> shouldExit(false);

struct Resource {
    int* buffer;
    Resource() {
        buffer = new int[1000000];  // 分配大块内存
        std::cout << "Resource created\n";
    }
    void use() {
        for (int i = 0; i < 1000000; ++i) {
            buffer[i] = i;  // 写入大块内存，加剧访问风险
        }
        std::cout << "使用资源完成\n";
    }
    ~Resource() {
        delete[] buffer;
        std::cout << "Resource destroyed\n";
    }
};

Resource* globalRes = new Resource();

void worker() {
    while (!shouldExit) {
        Resource* local = globalRes;  // 引用全局指针，但无任何保护
        if (local) {
            local->use();  // 有可能访问的是已 delete 的资源
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 高频率访问
    }
}

void asyncDestructorThread() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 给 worker 一点时间
    std::cout << "开始删除资源...\n";
    delete globalRes;
    globalRes = nullptr;
    std::cout << "资源已删除\n";
}

int main() {
    std::thread t(worker);

    std::thread deleter(asyncDestructorThread);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    shouldExit = true;

    t.join();
    deleter.join();

    std::cout << "程序正常退出（如果没崩溃）\n";
}
