/*
    背景：游戏开发中的粒子系统

    单线程无并发问题，不需考虑线程安全
    在设计对象池时可省去锁和原子操作，提高性能，减少开销。
    
    关注点：如何更高效地管理对象，减少内存分配次数，提升程序运行速度。

    如何在单线程中预分配对象、复用对象，以及如何管理对象的生命周期。


    考虑单线程下对象池的实现细节
        比如如何跟踪空闲对象，如何高效地分配和回收对象。可能需要使用栈或队列来管理空闲对象，确保快速获取和归还。

    对象池的扩容策略感兴趣，比如在单线程下是否可以采用不同的扩容方式，比如按需扩容而不是固定倍数，或者是否可以在不需要时缩减池的大小以节省内存。


    关键优化点
        预分配减少开销：初始创建5个对象，后续请求直接复用。
        动态按需扩容：当需求超过预分配数量时，自动创建新对象（例如示例中创建了8个粒子，其中3个是动态新增的）。
        无锁操作：单线程下无需同步机制，提升性能。

    性能对比
        操作	      传统 new/delete	        对象池
        内存分配次数	每次创建均需分配	    预分配 + 按需扩容
        内存碎片	        高	                  低
        对象创建耗时	    高（系统调用）      低（复用内存）
        适用场景	        低频创建	        高频创建/销毁

    进阶优化
    批量预分配：根据历史使用峰值调整初始大小，减少运行时扩容。
    惰性销毁：在池中保留一定数量的空闲对象，避免频繁扩容/缩容。
    内存布局优化：将对象存储在连续内存中，提升缓存命中率。


    在单线程环境中，对象池通过预分配和复用机制显著减少内存管理开销，尤其适合高频创建对象的场景。实现时需关注：

        预分配策略：平衡内存占用和性能。
        生命周期管理：确保对象状态正确重置。
        数据结构选择：如 std::deque 避免内存重分配。
 */

#pragma once

#include <vector>
#include <memory>

#include "particle.hpp"

class ParticlePool {
public:
    // 初始化预分配对象数量
    ParticlePool(size_t initialSize) {
        for (size_t i = 0; i < initialSize; ++i) {
            freeParticles_.push_back(std::make_unique<Particle>());
        }
    }

    // 获取一个粒子对象
    std::unique_ptr<Particle> acquire(float x, float y) {
        if (freeParticles_.empty()) {
            // 无空闲对象时动态扩容（单线程无需锁）
            auto newParticle = std::make_unique<Particle>();
            newParticle->reset(x, y);
            return newParticle;
        } else {
            // 复用空闲对象
            auto particle = std::move(freeParticles_.back());
            freeParticles_.pop_back();
            particle->reset(x, y);
            return particle;
        }
    }

    // 归还粒子对象到池中
    void release(std::unique_ptr<Particle> particle) {
        particle->active = false;
        freeParticles_.push_back(std::move(particle));
    }

private:
    std::vector<std::unique_ptr<Particle>> freeParticles_; // 空闲对象列表
};
