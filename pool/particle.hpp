#pragma once

#include <iostream>
#include <vector>
#include <memory>

class Particle {
public:
    float x, y;     // 粒子坐标
    bool active;     // 是否激活

    Particle() : x(0.0f), y(0.0f), active(false) {
        std::cout << "Particle created.\n";
    }

    // 重置粒子状态
    void reset(float x_, float y_) {
        x = x_;
        y = y_;
        active = true;
    }

    // 模拟粒子更新
    void update() {
        if (active) {
            y += 0.1f; // 简单模拟下落
            std::cout << "Particle at (" << x << ", " << y << ")\n";
        }
    }

    ~Particle() {
        std::cout << "Particle destroyed.\n";
    }
};
