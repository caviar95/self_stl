#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

// 定义两个互斥量
std::mutex mutexA;
std::mutex mutexB;

void deadlock_version() {
    std::thread t1([] {
        std::lock_guard<std::mutex> lockA(mutexA);
        std::cout << "[T1] Locked mutexA\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lockB(mutexB);  // 等待 mutexB
        std::cout << "[T1] Locked mutexB\n";
    });

    std::thread t2([] {
        std::lock_guard<std::mutex> lockB(mutexB);
        std::cout << "[T2] Locked mutexB\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lockA(mutexA);  // 等待 mutexA
        std::cout << "[T2] Locked mutexA\n";
    });

    t1.join();
    t2.join();
}

void no_deadlock_version() {
    std::thread t1([] {
        std::unique_lock<std::mutex> lockA(mutexA, std::defer_lock);
        std::unique_lock<std::mutex> lockB(mutexB, std::defer_lock);
        std::lock(lockA, lockB);  // 同时加锁，避免死锁
        std::cout << "[T1] Locked mutexA and mutexB safely\n";
    });

    std::thread t2([] {
        std::unique_lock<std::mutex> lockA(mutexA, std::defer_lock);
        std::unique_lock<std::mutex> lockB(mutexB, std::defer_lock);
        std::lock(lockA, lockB);  // 同样顺序加锁
        std::cout << "[T2] Locked mutexA and mutexB safely\n";
    });

    t1.join();
    t2.join();
}

int main() {
    std::cout << "1. 死锁版本开始:\n";
    // std::thread deadlock_thread([] {
    //     deadlock_version();
    // });

    deadlock_version();

    // 给死锁版本 3 秒时间卡住（演示用）
    // std::this_thread::sleep_for(std::chrono::seconds(3));

    // std::cout << "\n2. 无死锁版本开始:\n";
    // no_deadlock_version();

    // deadlock_thread.join(); // 等待死锁线程（实际上它可能永远卡住）
    return 0;
}
