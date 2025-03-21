#pragma once

#include <cassert>
#include <utility>
#include <stdexcept>

class Any {
public:
    Any() = default;

    template <typename T>
    Any(T&& val) : ptr(new Derived<std::decay_t<T>>(std::forward<T>(val))) {}

    ~Any() { delete ptr; }

    // 拷贝构造函数
    Any(const Any& other) : ptr(other.ptr ? other.ptr->clone() : nullptr) {}

    // 拷贝赋值运算符
    Any& operator=(const Any& other) {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr ? other.ptr->clone() : nullptr;
        }
        return *this;
    }

    // 移动构造函数
    Any(Any&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    // 移动赋值运算符
    Any& operator=(Any&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    template <typename T>
    T cast() const {
        if (auto d = dynamic_cast<Derived<T>*>(ptr)) {
            return d->value;
        }
        throw std::bad_cast(); // 类型不匹配时抛出异常
    }

private:
    struct Base {
        virtual ~Base() = default;
        virtual Base* clone() const = 0;
    };

    template <typename T>
    struct Derived : Base {
        T value;
        Derived(T v) : value(std::move(v)) {}

        Base* clone() const override {
            return new Derived(value);
        }
    };

    Base* ptr = nullptr;
};