// 基于C++的线程池实现，包含任务窃取机制、避免锁竞争的核心代码及测试方案

#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <random>
#include <algorithm>
#include <iostream>
#include <chrono>

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t num_threads) 
        : stop(false), worker_data(num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this, i] { worker_loop(i); });
        }
    }

    ~ThreadPool() {
        stop.store(true);
        for (auto& t : workers) t.join();
    }

    void submit(Task task) {
        // 轮询选择工作线程
        size_t index = next_worker.fetch_add(1) % worker_data.size();
        auto& worker = worker_data[index];
        std::lock_guard<std::mutex> lock(worker.queue_mutex);
        worker.task_queue.push_back(std::move(task));
    }

private:
    struct WorkerData {
        std::deque<Task> task_queue;
        std::mutex queue_mutex;
    };

    std::vector<WorkerData> worker_data;
    std::vector<std::thread> workers;
    std::atomic<bool> stop;
    std::atomic<size_t> next_worker{0};

    void worker_loop(size_t worker_id) {
        WorkerData& my_data = worker_data[worker_id];
        std::random_device rd;
        std::mt19937 gen(rd());

        while (!stop.load()) {
            Task task = get_local_task(my_data);
            
            if (!task) {
                task = steal_remote_task(worker_id, gen);
            }

            if (task) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

    Task get_local_task(WorkerData& data) {
        std::lock_guard<std::mutex> lock(data.queue_mutex);
        if (!data.task_queue.empty()) {
            Task task = std::move(data.task_queue.front());
            data.task_queue.pop_front();
            return task;
        }
        return nullptr;
    }

    Task steal_remote_task(size_t worker_id, std::mt19937& gen) {
        std::uniform_int_distribution<size_t> dist(0, worker_data.size()-1);
        size_t start = dist(gen);
        
        for (size_t i = 0; i < worker_data.size(); ++i) {
            size_t idx = (start + i) % worker_data.size();
            if (idx == worker_id) continue;

            auto& target = worker_data[idx];
            std::unique_lock<std::mutex> lock(target.queue_mutex, std::try_to_lock);
            
            if (lock.owns_lock() && !target.task_queue.empty()) {
                Task task = std::move(target.task_queue.back());
                target.task_queue.pop_back();
                return task;
            }
        }
        return nullptr;
    }
};

// 测试方案
void performance_test() {
    constexpr int NUM_TASKS = 100000;
    constexpr int NUM_THREADS = 8;
    
    ThreadPool pool(NUM_THREADS);
    std::atomic<int> counter{0};
    
    auto start = std::chrono::high_resolution_clock::now();

    // 提交任务
    for (int i = 0; i < NUM_TASKS; ++i) {
        pool.submit([&] {
            // 模拟IO密集型任务
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            counter.fetch_add(1);
        });
    }

    // 等待任务完成
    while (counter.load() < NUM_TASKS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Processed " << NUM_TASKS << " tasks in " 
              << duration.count() << " ms using "
              << NUM_THREADS << " threads" << std::endl;
}

void stealing_effect_test() {
    constexpr int NUM_TASKS = 10000;
    constexpr int NUM_THREADS = 4;
    
    ThreadPool pool(NUM_THREADS);
    std::mutex cout_mutex;
    std::vector<int> task_counts(NUM_THREADS, 0);

    // 将所有任务提交到第一个工作线程
    for (int i = 0; i < NUM_TASKS; ++i) {
        pool.submit([&, i] {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                // 记录任务被哪个线程执行
                static thread_local int executed_by = -1;
                if (executed_by == -1) {
                    executed_by = std::hash<std::thread::id>{}(std::this_thread::get_id()) % NUM_THREADS;
                }
                task_counts[executed_by]++;
            }
        });
    }

    // 等待任务完成
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\nTask distribution:\n";
    for (int i = 0; i < NUM_THREADS; ++i) {
        std::cout << "Thread " << i << " processed " 
                  << task_counts[i] << " tasks\n";
    }
}

int main() {
    std::cout << "Performance test:\n";
    performance_test();

    std::cout << "\nStealing effect test:\n";
    stealing_effect_test();
    return 0;
}

