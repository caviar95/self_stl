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

