/*
    1 任务封装与优先级比较：
        使用Task结构体封装任务，包含优先级和可调用对象。
    通过重载operator<实现最大堆，确保高优先级任务先出队。

    2 线程安全队列：
    使用std::priority_queue存储任务，配合std::mutex和std::condition_variable保证线程安全。
    在任务入队和出队时通过互斥锁确保同步。

    3 工作线程管理：
    工作线程持续从队列中获取任务，无任务时通过条件变量等待。
    析构时设置停止标志并唤醒所有线程，等待线程结束。

    4 完美转发与future支持：
    使用模板和std::forward实现参数的完美转发。
    通过std::packaged_task和std::future支持异步获取任务结果。


    注意：
        1 由于线程调度的不确定性，输出顺序可能因运行环境而异，但高优先级任务通常会更早完成
        2 异常处理：任务中的异常会通过std::future传递，需用户自行处理。
        3 资源管理：确保所有任务在析构前完成，避免资源泄漏。
        4 线程数调整：可根据实际需求调整工作线程数量以优化性能。
 */


#include "thread_pool_with_pri.hpp"

#include "gtest/gtest.h"

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

TEST(ThreadPoolWithPriTest, Basic) {
    ThreadPool pool(4);
    std::vector<int> results(100);
    for (int i = 0; i < 100; ++i) {
        pool.enqueue(i, [i, &results] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            results[i] = i;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(results[i], i);
    }
}

TEST(ThreadPoolWithPriTest, Exception) {
    ThreadPool pool(4);
    std::vector<int> results(100);
    for (int i = 0; i < 100; ++i) {
        pool.enqueue(i, [i, &results] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (i == 10) {
                throw std::runtime_error("exception");
            }
            results[i] = i;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (int i = 0; i < 100; ++i) {
        if (i == 10) {
            EXPECT_EQ(results[i], 0);
        } else {
            EXPECT_EQ(results[i], i);
        }
    }
}

TEST(ThreadPoolWithPriTest, Future) {
    ThreadPool pool(4);
    std::vector<std::future<int>> results(100);
    for (int i = 0; i < 100; ++i) {
        results[i] = pool.enqueue(i, [i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return i;
        });
    }

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(results[i].get(), i);
    }
}

