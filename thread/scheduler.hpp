#pragma once

#ifndef THREAD_POOL_WITH_PRI

#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <optional>

#include "thread_pool.hpp"

class Scheduler {
public:
    explicit Scheduler(ThreadPool &pool) : pool(pool), running(true) {
        scheduler_thread = std::thread([this] { run(); });
    }

    // void ScheduleAfter(std::chrono::milliseconds delay, std::function<void()> task) {
    //     auto time = std::chrono::steady_clock::now() + delay;
    //     std::lock_guard<std::mutex> lock(queue_mutex);
    //     queue.emplace(time, std::move(task));
    //     cv.notify_one(); // 只需要唤醒一个等待的线程来处理任务
    // }

    void Schedule(std::chrono::milliseconds interval, std::function<void()> task, int repeat_count = 1) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        auto first_run = std::chrono::steady_clock::now() + interval;
        queue.emplace(first_run, interval, std::move(task), repeat_count);
        cv.notify_one();
    }

    ~Scheduler() {
        running = false;
        cv.notify_all();
        if (scheduler_thread.joinable()) {
            scheduler_thread.join();
        }
    }

private:
    struct SceduledTask {
        std::chrono::steady_clock::time_point next_run;
        std::chrono::milliseconds interval;
        std::function<void()> task;
        int remaining;

        bool operator<(const SceduledTask& other) const {
            return next_run > other.next_run;
        }

        bool reschedule() {
            if (remaining == 0) {
                return false;
            }

            if (remaining > 0) {
                remaining--;
            }

            next_run += interval;
            return remaining != 0;
        }
    };

    std::priority_queue<SceduledTask> queue;
    std::mutex queue_mutex;
    std::condition_variable cv;
    ThreadPool& pool;
    std::thread scheduler_thread;
    bool running;

    // void run() {
    //     while (running) {
    //         std::unique_lock<std::mutex> lock(queue_mutex);
    //         if (queue.empty()) {
    //             cv.wait(lock);
    //             continue;
    //         }

    //         auto next_time = queue.top().time;
    //         if (cv.wait_until(lock, next_time) == std::cv_status::timeout) {
    //             std::optional<std::function<void()>> task = std::move(queue.top().task); // 不能直接移动，否则可能导致未定义行为，应先复制，再移动
    //             queue.pop();
    //             lock.unlock(); // lock.unlock() 后直接调用 pool.AddTask，可能导致锁释放不及时，影响性能。
    //             if (task) {
    //                 pool.AddTask(std::move(*task));
    //             }
    //         }
    //     }
    // }

    void run() {
        while (running) {

            std::unique_lock<std::mutex> lock(queue_mutex);
            if (queue.empty()) {
                cv.wait(lock);
                continue;
            }

            auto &task = queue.top();
            if (cv.wait_until(lock, task.next_run) == std::cv_status::timeout) {
                auto task_copy = task;
                queue.pop();

                lock.unlock();

                pool.AddTask(task_copy.task);

                if (task_copy.reschedule()) {
                    lock.lock();
                    queue.push(std::move(task_copy));
                    lock.unlock();
                }

                cv.notify_one();
            }
        }
    }
};

#endif
