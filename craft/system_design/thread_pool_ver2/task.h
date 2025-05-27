#pragma once

#include <functional>

struct Task {
    int priority;                         // 越小越优先
    std::function<void()> func;

    Task(int p, std::function<void()> f)
        : priority(p), func(std::move(f)) {}

    // 注意：priority_queue 是最大堆，需反向比较
    bool operator<(const Task &other) const {
        return this->priority > other.priority;
    }
};
