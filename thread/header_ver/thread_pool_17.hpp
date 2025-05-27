#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queue_mutex_;
    std::condition_variable condition_;

    std::atomic<bool> stop_;
};

inline ThreadPool::ThreadPool(size_t threads) : stop_(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers_.emplace_back([this]() {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lk(this->queue_mutex_);
                    this->condition_.wait(lk, [this]() {return this->stop_.load() || !this->tasks_.empty();});

                    if (this->stop_.load() && this->tasks_.empty()) {
                        return;
                    }

                    task = std::move(this->tasks_.front());

                    this->tasks_.pop();
                }

                try {
                    task();
                } catch (...) {
                    // log or rethrow
                }
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    stop_.store(true);
    condition_.notify_all();
    for (std::thread &worker : workers_) {
        worker.join();
    }
}

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();

    {
        std::lock_guard<std::mutex> lk(queue_mutex_);
        if (stop_.load()) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks_.emplace([task](){(*task)();});
    }

    condition_.notify_one();
    return res;
}
