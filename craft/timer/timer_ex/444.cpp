// main.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <atomic>
#include <chrono>
#include <map>
#include <memory>

using namespace std::chrono;

using Task = std::function<void()>;

//-----------------------------------
// ThreadPool
//-----------------------------------
class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this, i] {
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueue(Task task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (auto &worker : workers) worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

//-----------------------------------
// TimerScheduler
//-----------------------------------
struct TimerTask {
    time_point<steady_clock> execTime;
    milliseconds interval;
    Task task;
    bool repeat;
    int priority;
    size_t id;

    bool operator<(const TimerTask& other) const {
        return execTime > other.execTime || (execTime == other.execTime && priority < other.priority);
    }
};

class TimerScheduler {
public:
    TimerScheduler(ThreadPool& pool) : threadPool(pool), running(true) {
        schedulerThread = std::thread([this] { this->run(); });
    }

    ~TimerScheduler() {
        running = false;
        cv.notify_one();
        if (schedulerThread.joinable()) schedulerThread.join();
    }

    size_t addTask(milliseconds delay, milliseconds interval, Task task, bool repeat = true, int priority = 0) {
        TimerTask timerTask{
            steady_clock::now() + delay,
            interval,
            std::move(task),
            repeat,
            priority,
            ++taskIdCounter
        };

        {
            std::lock_guard<std::mutex> lock(mutex);
            taskQueue.push(timerTask);
        }
        cv.notify_one();
        return timerTask.id;
    }

private:
    void run() {
        while (running) {
            std::unique_lock<std::mutex> lock(mutex);
            if (taskQueue.empty()) {
                cv.wait(lock);
                continue;
            }

            auto now = steady_clock::now();
            if (taskQueue.top().execTime > now) {
                cv.wait_until(lock, taskQueue.top().execTime);
                continue;
            }

            auto task = taskQueue.top();
            taskQueue.pop();

            threadPool.enqueue([task] {
                auto tid = std::this_thread::get_id();
                auto now = system_clock::to_time_t(system_clock::now());
                std::cout << "[Timer Trigger] Task ID: " << task.id
                          << " | Executed by TID: " << tid
                          << " | Time: " << std::ctime(&now);
                task.task();
            });

            if (task.repeat) {
                task.execTime = now + task.interval;
                taskQueue.push(task);
            }
        }
    }

    ThreadPool& threadPool;
    std::priority_queue<TimerTask> taskQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::thread schedulerThread;
    std::atomic<bool> running;
    std::atomic<size_t> taskIdCounter = 0;
};

//-----------------------------------
// Example Usage
//-----------------------------------
void registerTasks(TimerScheduler& scheduler, int threadId, int intervalSec) {
    scheduler.addTask(
        milliseconds(intervalSec * 1000),
        milliseconds(intervalSec * 1000),
        [threadId] {
            std::cout << "Worker Task from logical Thread " << threadId << "\n";
        },
        true, 1);
}

int main() {
    ThreadPool pool(4);
    TimerScheduler scheduler(pool);

    for (int i = 1; i <= 4; ++i) {
        registerTasks(scheduler, i, i); // 1s, 2s, 3s, 4s interval tasks
    }

    std::this_thread::sleep_for(seconds(15));
    return 0;
}
