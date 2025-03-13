#pragma once

template <typename T>
class ObjectManager {
private:
    std::vector<std::unique_ptr<T, std::function<void(T*)>>> objects_; // 管理智能指针

public:
    // 创建对象
    template <typename... Args>
    void add(Args&&... args) {
        auto obj = PooledObject<T>::create(std::forward<Args>(args)...);
        objects_.push_back(std::move(obj));
    }

    // 删除对象（通过索引）
    void remove(size_t index) {
        if (index < objects_.size()) {
            objects_.erase(objects_.begin() + index);
        }
    }

    // 获取对象引用（只读）
    const T& get(size_t index) const {
        return *objects_.at(index);
    }

    // 更新对象（通过索引）
    template <typename... Args>
    void update(size_t index, Args&&... args) {
        if (index < objects_.size()) {
            *objects_[index] = T(std::forward<Args>(args)...); // 假设 T 支持赋值操作
        }
    }

    // 清空所有对象
    void clear() {
        objects_.clear();
    }

    // 遍历所有对象
    void forEach(std::function<void(const T&)> callback) const {
        for (const auto& obj : objects_) {
            callback(*obj);
        }
    }
};
