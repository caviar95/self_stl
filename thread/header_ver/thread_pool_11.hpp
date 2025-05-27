#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <memory>
#include <type_traits>

class ThreadPool {
public:
    explicit ThreadPool(size_t);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
};

inline ThreadPool::ThreadPool(size_t threads) : stop_(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers_.emplace_back([this]() {
            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lk(this->queue_mutex_);
                    this->condition_.wait(lk, [this](){return this->stop_ || !this->tasks_.empty();});

                    if (this->stop_ && this->tasks_.empty()) {
                        return;
                    }

                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }

                task();
            }
        });
    }
}


template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
//    using return_type = typename std::result_of<decltype(f)(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
        std::lock_guard<std::mutex> lk(queue_mutex_);

        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks_.emplace([task](){(*task)();});
    }

    condition_.notify_one();
    return res;
}

/*
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        [func = std::forward<F>(f), args_tuple = std::make_tuple(std::forward<Args>(args)...]() mutable {
            return std::apply(func, args_tuple);
        }
    );

    std::future<return_type> res = task->get_future();

    {
        std::lock_guard<std::mutex> lk(queue_mutex_);
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks_.emplace_back([task](){ (*task)(); });
    }

    condition_.notify_one();
    return res;
}
*/

inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lk(queue_mutex_);
        stop_ = true;
    }

    condition_.notify_all();

    for (std::thread &worker : workers_) {
        worker.join();
    }
}

