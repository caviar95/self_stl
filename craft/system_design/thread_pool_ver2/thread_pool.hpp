#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <chrono>

#include "task.h"

class ThreadPool {
public:
    ThreadPool(size_t threadCount) : stop(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    Task task(0, []{});

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });

                        if (stop && tasks.empty()) return;

                        task = std::move(tasks.top());
                        tasks.pop();
                    }

                    task.func(); // 执行任务
                }
            });
        }
    }

    // 新增：带优先级的任务提交
    void enqueue(std::function<void()> func, int priority = 10) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.emplace(priority, std::move(func));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();

        for (auto &worker : workers)
            if (worker.joinable()) worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};