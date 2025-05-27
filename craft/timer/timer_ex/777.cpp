#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <atomic>
#include <chrono>

struct TaskQueue {
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
};

class FixedWorkerPool {
public:
    FixedWorkerPool(size_t threadCount) : queues(threadCount), stop(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back(&FixedWorkerPool::workerLoop, this, i);
        }
    }
    
    ~FixedWorkerPool() {
        stop = true;

        for (auto &q : queues) {
            q.cv.notify_all();
        }

        for (auto &t : workers) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void submitTo(size_t threadId, std::function<void()> task) {
        if (threadId >= queues.size()) {
            throw std::out_of_range("Invalid threadId");
        }

        {
            std::lock_guard<std::mutex> lk(queues[threadId].mutex);
            queues[threadId].tasks.push(std::move(task));
        }

        queues[threadId].cv.notify_one();
    }

private:
    std::vector<TaskQueue> queues;
    std::vector<std::thread> workers;
    std::atomic<bool> stop;

    void workerLoop(size_t threadId) {
        int threadLocalData = 100 + static_cast<int>(threadId);

        while (!stop) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lk(queues[threadId].mutex);

                queues[threadId].cv.wait(lk, [&]() {
                    return stop || !queues[threadId].tasks.empty();
                });

                if (stop && queues[threadId].tasks.empty()) {
                    break;
                }

                task = std::move(queues[threadId].tasks.front());
                queues[threadId].tasks.pop();
            }

            taskWithLocalData(task, threadLocalData, threadId);
        }
    }

    void taskWithLocalData(std::function<void()> &task, int &localData, size_t threadId) {
        std::cout << "[Thread " << threadId << "] executing, thread local variable: " << localData << std::endl;
    }
};

void timerThreadFunc(FixedWorkerPool &pool, size_t targetThreadId, std::atomic<bool> &running) {
    int count{};

    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto timedTask = [count]() {
            std::cout << " >> timed task executing, count: " << count << std::endl;
        };

        pool.submitTo(targetThreadId, timedTask);

        ++count;
    }
}

int main() {
    constexpr size_t workerCount = 3;

    FixedWorkerPool pool(workerCount);

    std::atomic<bool> timerRunning(true);

    std::thread timerThread(timerThreadFunc, std::ref(pool), 1, std::ref(timerRunning));

    for (int i = 0; i < 10; ++i) {
        int threadId = i % workerCount;
        pool.submitTo(threadId, [i, threadId]() {
            std::cout << "[Thread " << threadId << "] execute normal task, index: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    timerRunning = false;
    timerThread.join();

    std::cout << "Procedure end" << std::endl;

    return 0;
}
