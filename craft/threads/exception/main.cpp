#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

std::atomic<bool> shouldExit(false);

// 模拟共享资源
struct Resource {
    int* arr = new int[100];

    void use() {
        for (int i = 0; i < 100; ++i) {
            arr[i] = i;  // 写操作更容易触发崩溃
        }
        std::cout << "写入资源..." << std::endl;
    }

    ~Resource() {
        delete[] arr;
        std::cout << "[资源已析构]" << std::endl;
    }
};


Resource* globalRes = new Resource();

void worker() {
    while (!shouldExit) {
        if (globalRes) {
            globalRes->use();  // 💥 如果资源已被回调线程析构，这里会 coredump
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 模拟回调线程释放资源
void onCallback() {
    std::cout << "[回调] 异常触发，准备析构资源..." << std::endl;

    // 拉起新线程释放资源
    std::thread([=]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 模拟延迟
        delete globalRes;
        globalRes = nullptr;
        std::cout << "[回调线程] 资源已被释放！" << std::endl;
    }).detach();
}

int main() {
    // std::thread t1(worker);

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    onCallback();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    shouldExit = true;

    // t1.join();
    for (auto i = 0; i < 10; ++i) {
        threads[i].join();
    }
    std::cout << "程序结束" << std::endl;
}
