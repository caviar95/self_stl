#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <functional>
#include <atomic>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iomanip>

using namespace std::chrono;

// ============ Logger ============

std::string now_time() {
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%F %T");
    return oss.str();
}

void log(const std::string& msg) {
    std::cout << "[" << now_time() << "] " << msg << std::endl;
}

// ============ Task ============

using TaskFunc = std::function<void()>;
using TaskID = uint64_t;

struct Task {
    TaskID id;
    TaskFunc func;
    int priority;
    int threadAffinity; // -1 means any
    Task(TaskID id_, TaskFunc f, int p, int threadId = -1)
        : id(id_), func(std::move(f)), priority(p), threadAffinity(threadId) {}
};

// ============ Thread-safe Queue ============

template <typename T>
class SafeQueue {
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<T> q;

public:
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            q.push(std::move(item));
        }
        cv.notify_one();
    }

    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return !q.empty(); });
        item = std::move(q.front());
        q.pop();
        return true;
    }
};

// ============ ThreadPool ============

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::unordered_map<int, std::shared_ptr<SafeQueue<Task>>> taskQueues;
    std::atomic<bool> stop{false};
    TaskID nextTaskId{0};

public:
    ThreadPool(int numThreads) {
        for (int i = 0; i < numThreads; ++i) {
            auto q = std::make_shared<SafeQueue<Task>>();
            taskQueues[i] = q;

            workers.emplace_back([i, q, this]() {
                while (!stop) {
                    Task task(0, nullptr, 0);
                    if (q->pop(task)) {
                        log("Thread " + std::to_string(i) + " executing Task " + std::to_string(task.id));
                        task.func();
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        stop = true;
        for (auto& [_, q] : taskQueues)
            q->push(Task(0, [] {}, 0)); // dummy task to unblock threads
        for (auto& t : workers)
            t.join();
    }

    TaskID submit(TaskFunc func, int priority = 0, int threadId = -1) {
        TaskID id = ++nextTaskId;
        Task task(id, std::move(func), priority, threadId);
        if (threadId >= 0 && taskQueues.count(threadId)) {
            taskQueues[threadId]->push(task);
        } else {
            // Round-robin or default: pick thread 0
            taskQueues[0]->push(task);
        }
        return id;
    }
};

// ============ TimerScheduler ============

struct TimerTask {
    TaskID id;
    steady_clock::time_point execTime;
    milliseconds interval;
    TaskFunc func;
    bool repeat;
    int priority;
    int threadAffinity;
    std::atomic<bool> cancelled;

    TimerTask(TaskID i, steady_clock::time_point et, milliseconds inter, TaskFunc f,
              bool r, int p, int ta)
        : id(i), execTime(et), interval(inter), func(std::move(f)),
          repeat(r), priority(p), threadAffinity(ta), cancelled(false) {}

    bool operator>(const TimerTask& other) const {
        return execTime > other.execTime;
    }
};

class TimerScheduler {
private:
    std::priority_queue<TimerTask, std::vector<TimerTask>, std::greater<>> timerQueue;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};
    std::thread timerThread;
    ThreadPool& pool;
    std::unordered_map<TaskID, TimerTask> taskMap;

public:
    TimerScheduler(ThreadPool& p) : pool(p) {
        timerThread = std::thread([this]() { this->run(); });
    }

    ~TimerScheduler() {
        stop = true;
        cv.notify_all();
        if (timerThread.joinable())
            timerThread.join();
    }

    TaskID schedule(TaskFunc func, milliseconds delay, bool repeat, milliseconds interval,
                    int priority = 0, int threadAffinity = -1) {
        auto execTime = steady_clock::now() + delay;
        static TaskID globalId = 1;
        TaskID id = globalId++;

        TimerTask task(id, execTime, interval, func, repeat, priority, threadAffinity);
        {
            std::lock_guard<std::mutex> lock(mtx);
            timerQueue.push(task);
            taskMap[id] = task;
        }
        cv.notify_all();
        return id;
    }

    void cancel(TaskID id) {
        std::lock_guard<std::mutex> lock(mtx);
        if (taskMap.count(id)) {
            taskMap[id].cancelled = true;
        }
    }

private:
    void run() {
        while (!stop) {
            TimerTask task(0, steady_clock::now(), milliseconds(0), [] {}, false, 0, -1);

            {
                std::unique_lock<std::mutex> lock(mtx);
                if (timerQueue.empty()) {
                    cv.wait(lock);
                    continue;
                }

                task = timerQueue.top();
                auto now = steady_clock::now();
                if (task.execTime > now) {
                    cv.wait_until(lock, task.execTime);
                    continue;
                }

                timerQueue.pop();
                if (task.cancelled) {
                    taskMap.erase(task.id);
                    continue;
                }
            }

            log("[Timer Trigger] Task " + std::to_string(task.id));
            pool.submit(task.func, task.priority, task.threadAffinity);

            if (task.repeat && !task.cancelled) {
                task.execTime = steady_clock::now() + task.interval;
                std::lock_guard<std::mutex> lock(mtx);
                timerQueue.push(task);
            }
        }
    }
};

// ============ Main ============

int main() {
    ThreadPool pool(4);
    TimerScheduler scheduler(pool);

    // Worker 0 - log every 1s
    scheduler.schedule([=] {
        log("Worker 0 periodic task");
    }, 1000ms, true, 1000ms, 1, 0);

    // Worker 1 - log every 2s
    scheduler.schedule([=] {
        log("Worker 1 periodic task");
    }, 2000ms, true, 2000ms, 1, 1);

    // One-time task on Worker 2
    scheduler.schedule([=] {
        log("Worker 2 one-shot task");
    }, 3000ms, false, 0ms, 1, 2);

    std::this_thread::sleep_for(10s);
    return 0;
}
