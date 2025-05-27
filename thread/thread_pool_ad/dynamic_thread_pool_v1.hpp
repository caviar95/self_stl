#pragma once

#include <queue>
#include <vector>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <thread>

class DynamicThreadPool {
public:
    DynamicThreadPool(size_t fixedThreadCount, size_t maxThreads)
        : stopFlag_(false), fixedCount_(fixedThreadCount), maxThreads_(maxThreads) {
        for (size_t i = 0; i < fixedThreadCount; ++i) {
            workers_.emplace_back(&DynamicThreadPool::workerLoop, this, true);
        }
    }

    ~DynamicThreadPool() {
        stopFlag_ = true;
        cv_.notify_all();

        for (auto &t : workers_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void submit(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lk(queueMutex_);

            taskQueue_.push(std::move(task));
        }

        cv_.notify_one();

        maybeSpawnDynamicThread();
    }

private:
    std::queue<std::function<void()>> taskQueue_;
    std::vector<std::thread> workers_;
    std::mutex queueMutex_;
    std::condition_variable cv_;
    std::atomic<bool> stopFlag_;

    size_t fixedCount_;
    size_t maxThreads_;
    std::mutex dynamicMutex_;

    void workerLoop(bool isFixed) {
        while (!stopFlag_) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lk(queueMutex_);

                cv_.wait_for(lk, std::chrono::milliseconds(200), [&]{
                    return stopFlag_ || !taskQueue_.empty();
                });

                if (stopFlag_ && taskQueue_.empty()) break;

                if (!taskQueue_.empty()) {
                    task = std::move(taskQueue_.front());
                    taskQueue_.pop();
                } else {
                    if (!isFixed) {
                        break;
                    } else {
                        continue;
                    }
                }
            }

            task();
        }
    }

    void maybeSpawnDynamicThread() {
        std::lock_guard<std::mutex> lk(dynamicMutex_);

        if (workers_.size() < maxThreads_) {
            workers_.emplace_back(&DynamicThreadPool::workerLoop, this, false);
        }
    }
};
