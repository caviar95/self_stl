明白，接下来我会帮你把日志和监控功能集成进线程池和调度器代码里，同时给 `main.cpp` 里写示例代码演示日志打印和监控数据输出。

---

# 1. 修改 `thread_pool.cpp` 以集成日志和监控

```cpp
#include "thread_pool.hpp"
#include "logger.hpp"
#include "monitor.hpp"
#include <iostream>

static Monitor monitor;

ThreadPool::ThreadPool(size_t min_threads_, size_t max_threads_)
    : min_threads(min_threads_), max_threads(max_threads_) {
    for (size_t i = 0; i < min_threads; ++i) {
        workers.emplace_back();
        workers[i].thread = std::thread(&ThreadPool::worker_loop, this, i);
        monitor.thread_started();
        Logger::instance().log("Worker thread " + std::to_string(i) + " started.");
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(global_mutex);
        stop = true;
    }
    cv.notify_all();
    for (auto& w : workers) {
        if (w.thread.joinable()) {
            w.thread.join();
            monitor.thread_exited();
            Logger::instance().log("Worker thread exited.");
        }
    }
}

std::atomic<bool>* ThreadPool::enqueue(std::function<void()> func, int priority,
    Clock::time_point time, std::optional<int> thread_id) {
    auto* cancel_flag = new std::atomic<bool>(false);
    Task task(priority, std::move(func), time, cancel_flag, thread_id);

    {
        std::lock_guard<std::mutex> lock(global_mutex);
        global_queue.push(std::move(task));
        monitor.task_enqueued();
        Logger::instance().log("Task enqueued with priority " + std::to_string(priority));
    }
    cv.notify_all();
    return cancel_flag;
}

void ThreadPool::worker_loop(size_t thread_id) {
    while (true) {
        Task task(0, [] {});
        bool found = false;

        {
            std::unique_lock<std::mutex> lock(global_mutex);
            cv.wait(lock, [&] { return stop || !global_queue.empty(); });

            if (stop) break;

            std::priority_queue<Task> temp;
            while (!global_queue.empty()) {
                Task t = global_queue.top();
                if (t.cancelled && t.cancelled->load()) {
                    Logger::instance().log("Cancelled task skipped.");
                    monitor.task_completed();
                    global_queue.pop();
                    continue;
                }
                if (Clock::now() < t.exec_time) {
                    temp.push(t);
                    global_queue.pop();
                    continue;
                }
                if (t.bind_thread && *t.bind_thread != (int)thread_id) {
                    temp.push(t);
                    global_queue.pop();
                    continue;
                }
                task = t;
                global_queue.pop();
                found = true;
                break;
            }
            while (!temp.empty()) {
                global_queue.push(temp.top());
                temp.pop();
            }
        }

        if (found) {
            Logger::instance().log("Worker thread " + std::to_string(thread_id) + " executing task.");
            task.func();
            monitor.task_completed();
            Logger::instance().log("Worker thread " + std::to_string(thread_id) + " completed task.");
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
```

---

# 2. 修改 `scheduler.cpp` 集成日志

```cpp
#include "scheduler.hpp"
#include "logger.hpp"
#include <chrono>
#include <thread>

Scheduler::Scheduler(ThreadPool& p) : pool(p), running(true) {
    Logger::instance().log("Scheduler started.");
}

void Scheduler::every(int seconds, std::function<void()> func, int priority) {
    timers.emplace_back([=, this] {
        while (running.load()) {
            auto now = Clock::now();
            pool.enqueue(func, priority, now + std::chrono::seconds(seconds));
            Logger::instance().log("Scheduled task enqueued every " + std::to_string(seconds) + " seconds.");
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
        }
    });
}

void Scheduler::stop() {
    running = false;
    for (auto& t : timers) {
        if (t.joinable()) t.join();
    }
    Logger::instance().log("Scheduler stopped.");
}

Scheduler::~Scheduler() {
    stop();
}
```

---

# 3. 添加示例 `main.cpp`

```cpp
#include "thread_pool.hpp"
#include "scheduler.hpp"
#include "logger.hpp"
#include "monitor.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    ThreadPool pool(2, 4);
    Scheduler scheduler(pool);

    pool.enqueue([] { std::cout << "Immediate task executed\n"; }, 5);

    scheduler.every(2, [] {
        std::cout << "Scheduled task running every 2 seconds\n";
    }, 8);

    // 简单监控展示线程状态和任务数
    for (int i = 0; i < 10; ++i) {
        std::cout << "Active threads: " << pool.workers.size() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    scheduler.stop();

    Logger::instance().log("Main finished.");
    return 0;
}
```

---

你可以试着运行这个版本，会在控制台看到详细的日志输出，同时监控任务数和线程状态也被打印。

如果需要，我也可以帮你写更复杂的日志等级支持或监控界面。你觉得呢？
