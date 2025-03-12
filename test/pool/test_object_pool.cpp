#include <thread>

#include "object_pool.hpp"
#include "game.hpp"
#include "database_connection.hpp"
#include "gtest/gtest.h"


#ifndef OBJECT_POOL_MIN_MAX_SIZE

TEST(ObjectPool, Game) {
    // 创建对象池，初始容量为2
    ObjectPool<GameEntity> pool(2);

    // 从池中获取对象并设置属性
    auto entity1 = pool.allocate();
    entity1->reset(1001, 10.5f, 20.5f);
    EXPECT_EQ(entity1->id, 1001);
    EXPECT_EQ(entity1->x, 10.5f);
    EXPECT_EQ(entity1->y, 20.5f);

    auto entity2 = pool.allocate();
    entity2->reset(1002, 30.0f, 40.0f);

    EXPECT_EQ(entity2->id, 1002);
    EXPECT_EQ(entity2->x, 30.0f);
    EXPECT_EQ(entity2->y, 40.0f);

    // 归还对象到池中
    entity1.reset(); // 调用 deallocate
    entity2.reset();

    // 再次从池中获取对象（复用已归还的对象）
    auto entity3 = pool.allocate();
    entity3->reset(1003, 50.0f, 60.0f);
    EXPECT_EQ(entity3->id, 1003);
    EXPECT_EQ(entity3->x, 50.0f);
    EXPECT_EQ(entity3->y, 60.0f);
}

#else

TEST(ObjectPool, Dataconnection) {
    // 初始化对象池：最小2个连接，最大5个连接
    ObjectPool<DatabaseConnection> pool(2, 5);

    // 模拟多线程使用
    auto worker = [&pool](int threadId) {
        try {
            auto conn = pool.acquire(); // 获取连接
            std::cout << "Thread " << threadId 
                      << " using connection: " << conn->connectionId << "\n";
            // 模拟数据库操作
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // conn 超出作用域时，自动归还到池中
        } catch (const std::exception& e) {
            std::cerr << "Thread " << threadId << " error: " << e.what() << "\n";
        }
    };

    // 启动多个线程测试
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, i);
    }
    for (auto& t : threads) {
        t.join();
    }
}

#endif
