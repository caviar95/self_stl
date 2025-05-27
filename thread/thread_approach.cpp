#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> ready{false};
int task_counter = 0;

// 工作线程：等待任务并执行
void worker_thread() {
    while (true) {
        // ⏳ 自旋检查任务是否准备好（减少 wait 的频率）
        while (!ready.load()) {
            std::this_thread::yield(); // 主动让出 CPU
        }

        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return ready.load(); }); // 等待通知，并释放锁

        // 模拟任务处理
        std::cout << "[Worker] Executing task #" << task_counter << std::endl;
        ready = false;  // 标记任务完成

        if (task_counter >= 5) break;  // 结束条件
    }
}

// 主线程：周期性产生任务
void task_producer() {
    for (int i = 1; i <= 5; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟任务生成延迟

        {
            std::lock_guard<std::mutex> lock(mtx);
            task_counter = i;
            ready = true;
            std::cout << "[Main] Task #" << i << " ready\n";
        }

        cv.notify_one(); // 通知工作线程
    }
}

int main() {
    std::thread worker(worker_thread);
    std::thread producer(task_producer);

    producer.join();
    worker.join();

    std::cout << "All tasks finished.\n";
    return 0;
}
