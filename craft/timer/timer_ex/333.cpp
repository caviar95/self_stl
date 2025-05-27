// C++ 实现：一个定时器线程 + 4 个工作线程的任务执行系统
// 支持单次/多次/周期性执行，优先级，任务取消，延迟执行等

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <map>
#include <atomic>
#include <chrono>
#include <memory>

using namespace std::chrono;

using TaskFunc = std::function<void()>;
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

struct TimerTask {
    std::function<void()> callback;
    std::chrono::milliseconds interval;
    std::chrono::steady_clock::time_point nextRun;
    int repeat; // -1 = infinite
    int priority;
    std::atomic<bool> cancelled = false;

    TimerTask(std::function<void()> cb,
              std::chrono::milliseconds intv,
              int rep,
              int prio)
        : callback(std::move(cb)), interval(intv),
          nextRun(std::chrono::steady_clock::now() + intv),
          repeat(rep), priority(prio) {}

    bool operator<(const TimerTask& other) const {
        // reverse order for priority_queue
        if (nextRun == other.nextRun)
            return priority < other.priority;
        return nextRun > other.nextRun;
    }
};

struct TimerTaskComparator {
    bool operator()(const std::shared_ptr<TimerTask>& lhs,
                    const std::shared_ptr<TimerTask>& rhs) const {
        return *lhs < *rhs;
    }
};


class TimerScheduler {
public:
    TimerScheduler(std::vector<std::thread::id>& workerIds,
                   std::vector<std::queue<TaskFunc>>& taskQueues,
                   std::vector<std::mutex>& queueMutexes,
                   std::vector<std::condition_variable>& queueCVs)
        : taskQueues(taskQueues), queueMutexes(queueMutexes), queueCVs(queueCVs), workerIds(workerIds) {
        schedulerThread = std::thread(&TimerScheduler::run, this);
    }

    ~TimerScheduler() {
        stop = true;
        cv.notify_all();
        if (schedulerThread.joinable()) {
            schedulerThread.join();
        }
    }

    uint64_t schedule(TaskFunc func, milliseconds delay, milliseconds interval = milliseconds(0), size_t repeat = 1, int priority = 10) {
        std::unique_lock<std::mutex> lock(mtx);
        uint64_t id = ++nextId;
        auto task = std::make_shared<TimerTask>(TimerTask{
            id,
            Clock::now() + delay,
            func,
            repeat,
            interval,
            priority
        });

        tasks.push(task);
        taskMap[id] = task;
        cv.notify_all();
        return id;
    }

    void cancel(uint64_t id) {
        std::unique_lock<std::mutex> lock(mtx);
        if (taskMap.count(id)) {
            taskMap[id]->cancelled = true;
        }
    }

private:
    // std::priority_queue<std::shared_ptr<TimerTask>, std::vector<std::shared_ptr<TimerTask>>, std::greater<>> tasks;
    std::priority_queue<std::shared_ptr<TimerTask>, std::vector<std::shared_ptr<TimerTask>>, TimerTaskComparator> tasks;
    std::map<uint64_t, std::shared_ptr<TimerTask>> taskMap;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};
    std::thread schedulerThread;
    std::atomic<uint64_t> nextId{0};

    std::vector<std::thread::id>& workerIds;
    std::vector<std::queue<TaskFunc>>& taskQueues;
    std::vector<std::mutex>& queueMutexes;
    std::vector<std::condition_variable>& queueCVs;
    size_t roundRobin = 0;

    void run() {
        while (!stop) {
            std::unique_lock<std::mutex> lock(mtx);
            if (tasks.empty()) {
                cv.wait(lock);
                continue;
            }

            auto now = Clock::now();
            auto task = tasks.top();
            if (task->cancelled) {
                tasks.pop();
                taskMap.erase(task->id);
                continue;
            }

            if (task->executeAt <= now) {
                tasks.pop();
                size_t targetWorker = roundRobin++ % workerIds.size();
                {
                    std::unique_lock<std::mutex> qlock(queueMutexes[targetWorker]);
                    taskQueues[targetWorker].push(task->task);
                }
                queueCVs[targetWorker].notify_one();

                if (task->repeatCount > 1 || task->repeatCount == 0) {
                    if (task->repeatCount > 1) task->repeatCount--;
                    task->executeAt = now + task->interval;
                    tasks.push(task);
                } else {
                    taskMap.erase(task->id);
                }
            } else {
                cv.wait_until(lock, task->executeAt);
            }
        }
    }
};

void workerLoop(int workerId, std::queue<TaskFunc>& taskQueue, std::mutex& mtx, std::condition_variable& cv) {
    while (true) {
        TaskFunc task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return !taskQueue.empty(); });
            task = std::move(taskQueue.front());
            taskQueue.pop();
        }
        task();
    }
}

int main() {
    const int workerCount = 4;
    std::vector<std::thread> workers;
    std::vector<std::queue<TaskFunc>> taskQueues(workerCount);
    std::vector<std::mutex> queueMutexes(workerCount);
    std::vector<std::condition_variable> queueCVs(workerCount);
    std::vector<std::thread::id> workerIds(workerCount);

    for (int i = 0; i < workerCount; ++i) {
        workers.emplace_back([&, i] {
            workerIds[i] = std::this_thread::get_id();
            workerLoop(i, taskQueues[i], queueMutexes[i], queueCVs[i]);
        });
    }

    TimerScheduler scheduler(workerIds, taskQueues, queueMutexes, queueCVs);

    // 示例：设置 4 个不同间隔的周期任务
    for (int i = 0; i < workerCount; ++i) {
        int interval = (i + 1) * 1000;
        scheduler.schedule([i, interval] {
            auto now = std::chrono::system_clock::now();
            auto t = std::chrono::system_clock::to_time_t(now);
            std::cout << "[Worker Task] Worker " << i+1 << " executed at " << std::ctime(&t);
        }, milliseconds(interval), milliseconds(interval), 0);
    }

    std::this_thread::sleep_for(std::chrono::seconds(15));
    return 0;
}
