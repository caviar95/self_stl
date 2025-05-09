// test_allocator.cpp
#include "memory/simple_allocator.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <atomic>

// 生命周期跟踪类
class LifecycleTracker {
public:
    inline static std::atomic<int> constructions = 0;
    inline static std::atomic<int> destructions = 0;

    explicit LifecycleTracker(int v) : value(v) { ++constructions; }
    ~LifecycleTracker() { ++destructions; }
    
    int value;
};

// 测试基类
class SimpleAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        LifecycleTracker::constructions = 0;
        LifecycleTracker::destructions = 0;
    }
};

// 多线程 vector 测试（修复版）
TEST_F(SimpleAllocatorTest, MT_Vector) {
    using Vector = std::vector<int, SimpleAllocator<int>>;
    Vector vec;
    std::mutex vec_mutex;

    constexpr int THREADS = 4;
    constexpr int OPS = 1000;

    auto worker = [&]() {
        for (int i = 0; i < OPS; ++i) {
            {
                std::lock_guard<std::mutex> lock(vec_mutex);
                vec.push_back(i);
            }
            
            if (i % 100 == 0) {
                std::lock_guard<std::mutex> lock(vec_mutex);
                if (!vec.empty()) {
                    vec.pop_back();
                }
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < THREADS; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    // 验证基本容器完整性
    EXPECT_GE(vec.size(), THREADS * OPS - THREADS * (OPS/100 + 1));
}

// 严格的 list 生命周期测试
TEST_F(SimpleAllocatorTest, ListLifecycle) {
    using List = std::list<LifecycleTracker, SimpleAllocator<LifecycleTracker>>;
    
    {
        List lst;
        constexpr int NODES = 1000;
        for (int i = 0; i < NODES; ++i) {
            lst.emplace_back(i);
        }

        EXPECT_EQ(LifecycleTracker::constructions, NODES);
        EXPECT_EQ(LifecycleTracker::destructions, 0);
    }  // list 析构时应该销毁所有节点

    EXPECT_EQ(LifecycleTracker::destructions, 1000);
}
