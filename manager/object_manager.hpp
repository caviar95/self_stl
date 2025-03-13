#pragma once

template <typename T>
class ObjectManager {
public:
    template <typename... Args>
    T *add(Args &&...args) {
        T *obj = new T(std::forward<Args>(args)...);
        objects_.push_back(obj);
        return obj;
    }

private:
    std::vector<T*> objects_;
};
