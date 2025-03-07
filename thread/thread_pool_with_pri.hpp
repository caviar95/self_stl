#pragma once

/*
    Thread Pool 通常包括：任务队列、工作线程、添加任务的接口。
    考虑优先级，需要额外的优先级队列。[使用stl的priority_queue实现, 注意线程安全]

    任务的类型需要包含优先级参数，以及任务的执行函数。

    线程池的工作线程需要从队列中取出最高优先级的任务执行，队列的访问需要互斥锁以及通过条件变量通知线程。

    任务比较：默认最大堆，less

    定义
        std::priority_queue<Task, std::vector<Task>, Compare> tasks;

    enqueue 需要接受不同优先级的任务，需要使用模板，允许用户传入不同的可调用对象和参数，然后打包成Task结构体，加入队列
        用户传入的函数和参数需要绑定到std::function<void()>
            可以使用std::bind和lambda表达式
            需要处理参数的完美转发，以支持各种参数类型
        所以enqueue函数可能是一个可变参数模板函数，接受优先级/可调用对象和参数，然后打包成Task，加入队列
    

 */

#include <functional>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <future>
#include <stdexcept>
#include <memory>

/*
struct Task {
    int priority;
    std::function<void()> func;
};

// 优先级比较器，最大堆
struct Compare {
    bool operator()(const Task& a, const Task& b) {
        return a.priority < b.priority;
    }
};

// 可能需要处理异常以及是否返回future，如果需要获取任务的返回值，可能需要返回一个std::future
template <class F, class... Args>
void enqueue(int priority, F&& f, Args&&... args) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::bind(std::forward<f>, std::forward(args)...));

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(Task{priority, task});
    }

    condition.notify_one();
}

void worker() {
    while (true) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        condition.wait(lock, this); // 这是什么写法？
        if (stop && tasks.empty()) {
            return;
        }

        if (!tasks.empty()) {
            Task task = std::move(tasks.top());
            tasks.pop();
            lock.unlock();
            task.func();
        }
    }
}
*/

// 当有多个线程等待任务时，如果保证高优先级任务被优先取出
// 当任务被加入队列时，条件变量会通知一个线程，该线程会取出最高优先级的任务
// 如果有多个线程被唤醒，会竞争锁，第一个获得锁的线程会取出任务，其他线程可能发现队列为空或者有新任务，继续等待
// 优先级队列，高优先级任务先出队

// 当一个低优先级任务正在执行时，高优先级加入队列，此时不一定会立即执行，因为可能所有工作县城都在执行任务，只有当某个线程完成任务并重新进入等待时，才会取出高优先级任务。
// 如果线程池的线程数量足够，可以及时执行高优先级任务，否则可能需要等待。

// 线程池扩容？线程池缩容？线程池的线程数量如何确定？线程池的线程数量如何调整？

// 互斥锁的粒度需尽可能小，避免任务执行时占用锁，导致其他线程无法处理队列

// 考虑异常场景，如果任务执行过程中抛出异常，如何处理？ 任务执行函数是否需要try-catch？如果任务执行失败，是否需要重新执行？是否需要记录失败任务？
// 如果不捕获异常而终止，会导致线程池中线程数量减少
// 用户自己处理异常？线程池应该允许异常传播？
// 如果线程池捕获所有异常，可能导致问题被隐藏，如果不捕获，线程因异常退出，导致资源泄漏或线程数减少
// 捕获异常，并做适当处理，比如记录日志，但保证线程继续运行

// 线程池被销毁时，如何处理尚未执行的任务？等待所有任务完成？立即销毁线程池，丢弃任务？
// 析构中，设置stop标志，通知所有线程，执行完成当前任务后推出，剩余未执行的任务可能被丢弃，或者选择等待队列清空

class ThreadPool {
public:
    ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] { 
            while (true) {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                if (this->stop && this->tasks.empty()) {
                    return;
                }

                if (!this->tasks.empty()) {
                    Task task = std::move(this->tasks.top());
                    this->tasks.pop();
                    lock.unlock();
                    task.func();
                }
            }
        });}
    }

    ~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    // template <class F, class... Args>
    // void enqueue(int priority, F&& f, Args&&... args);

    template <class F, class... Args>
    auto enqueue(int priority, F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        // auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward(f), std::forward(args)...));
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace(Task{priority, [task] { (*task)(); }});
        }

        condition.notify_one();
        return res;
    }

private:
    struct Task {
        int priority;
        std::function<void()> func;
        bool operator<(const Task& other) const {
            return priority < other.priority; // 最大堆
        }
    };

    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};




/*
// std::bind直接包装可能无法正确处理可调用对象和参数类型，更好的方法可能是使用std::packaged_task来包装任务，并支持获取future，或者使用lambda来捕获参数
template <class F, class... Args>
void ThreadPool::enqueue(int priority, F&& f, Args&&... args) {
    // auto task_func = std::bind(std::forward(f), std::forward(args)...);

    // {
    //     std::unique_lock<std::mutex> lock(queue_mutex);
    //     tasks.emplace(Task{priority, std::function<void()>(task_func)});
    // }

    // condition.notify_one();

    // ver 2
    // auto func = std::make_shared<std::packaged_task<void()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    // {
    //     std::unique_lock<std::mutex> lock(queue_mutex);
    //     tasks.emplace(Task{priority, [func] { (*func)(); }});
    // }
    // condition.notify_one();

    // ver 3
    // 使用std::apply来展开参数包，并且使用tuple来保存参数的完美转发，至少C++17
    // auto task_func = [f = std::forward(f), args = std::make_tuple(std::forward(args)...)]() mutable {
    //     std::apply(std::move(f), std::move(args));
    // };

    // {
    //     std::unique_lock<std::mutex> lock(queue_mutex);
    //     tasks.emplace(Task{priority, std::function<void()>(task_func)});
    }

    // 每次添加任务都通知一个线程，而不是每次都唤醒所有线程，
    // 如果有多个任务被连续添加，可能每次添加都唤醒一个线程，导致多个线程竞争锁，
    // 但队列中的任务按优先级排序，所以第一个线程会取出最高优先级的任务，然后下一个线程取出次高，以此类推
    // condition.notify_one();
    condition.notify_all();
}
*/




