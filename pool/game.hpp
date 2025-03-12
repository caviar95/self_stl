#pragma once

#include <iostream>

// 示例类：游戏实体（包含属性）
class GameEntity {
public:
    int id;          // 成员属性：实体ID
    float x, y;      // 成员属性：坐标

    GameEntity() = default;

    // 构造函数（可自定义初始化）
    GameEntity(int id, float x, float y) : id(id), x(x), y(y) {
        std::cout << "GameEntity " << id << " constructed.\n";
    }

    // 重置对象状态（供对象池复用）
    void reset(int new_id, float new_x, float new_y) {
        id = new_id;
        x = new_x;
        y = new_y;
    }

    // 析构函数（仅用于演示）
    ~GameEntity() {
        std::cout << "GameEntity " << id << " destroyed.\n";
    }
};