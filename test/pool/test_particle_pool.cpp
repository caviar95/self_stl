#include "partical_pool.hpp"

#include "gtest/gtest.h"

TEST(ParticlePool, test1) {
    ParticlePool pool(5); // 初始预分配5个粒子

    // 爆发粒子（第一次使用）
    std::vector<std::unique_ptr<Particle>> activeParticles;
    for (int i = 0; i < 8; ++i) {
        auto p = pool.acquire(i * 1.0f, 0.0f);
        activeParticles.push_back(std::move(p));
    }

    EXPECT

    // 更新粒子状态
    for (auto& p : activeParticles) {
        p->update();
    }

    // 回收所有粒子
    for (auto& p : activeParticles) {
        pool.release(std::move(p));
    }
    activeParticles.clear();

    // 再次爆发粒子（复用已回收的）
    for (int i = 0; i < 3; ++i) {
        auto p = pool.acquire(i * 2.0f, 0.0f);
        activeParticles.push_back(std::move(p));
    }
}