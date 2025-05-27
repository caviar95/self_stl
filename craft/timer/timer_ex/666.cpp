#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <atomic>
#include <chrono>

// 线程安全任务队列
struct TaskQueue {
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
};

// 固定工作线程池
class FixedWorkerPool {
public:
    FixedWorkerPool(size_t threadCount)
        : queues(threadCount), stopFlag(false)
    {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back(&FixedWorkerPool::workerLoop, this, i);
        }
    }

    ~FixedWorkerPool() {
        stopFlag = true;
        // 唤醒所有线程
        for (auto& q : queues) {
            q.cv.notify_all();
        }
        for (auto& t : workers) {
            if (t.joinable()) t.join();
        }
    }

    // 向指定线程提交任务
    void submitTo(size_t threadId, std::function<void()> task) {
        if (threadId >= queues.size()) {
            throw std::out_of_range("Invalid threadId");
        }
        {
            std::lock_guard<std::mutex> lock(queues[threadId].mutex);
            queues[threadId].tasks.push(std::move(task));
        }
        queues[threadId].cv.notify_one();
    }

private:
    void workerLoop(size_t threadId) {
        // 模拟线程局部数据
        int threadLocalData = 100 + (int)threadId;

        while (!stopFlag) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queues[threadId].mutex);
                queues[threadId].cv.wait(lock, [&]() {
                    return stopFlag || !queues[threadId].tasks.empty();
                });
                if (stopFlag && queues[threadId].tasks.empty())
                    break;
                task = std::move(queues[threadId].tasks.front());
                queues[threadId].tasks.pop();
            }
            // 执行任务时，可以访问线程局部数据
            taskWithLocalData(task, threadLocalData, threadId);
        }
    }

    void taskWithLocalData(std::function<void()>& task, int& localData, size_t threadId) {
        // 这里我们传递线程局部数据给任务示例（模拟访问线程私有资源）
        std::cout << "[Thread " << threadId << "] 执行任务，线程局部数据：" << localData << std::endl;
        task();
    }

    std::vector<TaskQueue> queues;
    std::vector<std::thread> workers;
    std::atomic<bool> stopFlag;
};

// 定时器线程函数，周期触发任务投递
void timerThreadFunc(FixedWorkerPool& pool, size_t targetThreadId, std::atomic<bool>& running) {
    int count = 0;
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 每秒触发一次

        // 定时任务内容，必须放在线程池线程执行
        auto timedTask = [count]() {
            std::cout << "  >> 定时任务执行，计数：" << count << std::endl;
        };

        // 投递到指定工作线程任务队列
        pool.submitTo(targetThreadId, timedTask);

        ++count;
    }
}

int main() {
    constexpr size_t workerCount = 3;
    FixedWorkerPool pool(workerCount);

    std::atomic<bool> timerRunning(true);
    // 定时器线程将定时任务投递给线程1执行
    std::thread timerThread(timerThreadFunc, std::ref(pool), 1, std::ref(timerRunning));

    // 主线程模拟向各工作线程提交普通任务
    for (int i = 0; i < 10; ++i) {
        int threadId = i % workerCount;
        pool.submitTo(threadId, [i, threadId]() {
            std::cout << "[Thread " << threadId << "] 执行普通任务，编号：" << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));  // 模拟耗时
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    // 运行10秒后停止
    std::this_thread::sleep_for(std::chrono::seconds(10));
    timerRunning = false;
    timerThread.join();

    std::cout << "程序结束" << std::endl;
    return 0;
}
