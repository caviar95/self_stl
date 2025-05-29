// smart_ptr_impl.hpp
#pragma once
#include <cstddef>
#include <iostream>
#include <utility>
#include <memory>
#include <new>
#include <type_traits>
#include <atomic>

namespace smart {

// -------------------------
// unique_ptr with deleter
// -------------------------
template<typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr, Deleter deleter = Deleter{})
        : ptr_(ptr), deleter_(deleter) {}

    ~UniquePtr() {
        reset();
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    void reset(T* ptr = nullptr) {
        if (ptr_ && ptr_ != ptr) deleter_(ptr_);
        ptr_ = ptr;
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(deleter_, other.deleter_);
    }

    T* release() {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

private:
    T* ptr_;
    Deleter deleter_;
};

// -------------------------
// shared_ptr control block
// -------------------------
template<typename T>
struct ControlBlock {
    T* ptr;
    std::atomic<size_t> strong_count;
    std::atomic<size_t> weak_count;

    template<typename... Args>
    static ControlBlock* create(Args&&... args) {
        T* obj = new T(std::forward<Args>(args)...);
        try {
            return new ControlBlock(obj);
        } catch (...) {
            delete obj;
            throw;
        }
    }

    explicit ControlBlock(T* p)
        : ptr(p), strong_count(0), weak_count(0) {}

    ~ControlBlock() { delete ptr; }
};

// -------------------------
// forward declaration
// -------------------------
template<typename T>
class WeakPtr;

// -------------------------
// shared_ptr
// -------------------------
template<typename T>
class SharedPtr {
public:
    SharedPtr() : ptr_(nullptr), ctrl_(nullptr) {}

    explicit SharedPtr(T* ptr)
        : ptr_(ptr), ctrl_(ptr ? new ControlBlock<T>(ptr) : nullptr) {
        if (ctrl_) ctrl_->strong_count.fetch_add(1, std::memory_order_relaxed);
    }

    SharedPtr(const SharedPtr& other)
        : ptr_(other.ptr_), ctrl_(other.ctrl_) {
        if (ctrl_) ctrl_->strong_count.fetch_add(1, std::memory_order_relaxed);
    }

    SharedPtr(SharedPtr&& other) noexcept
        : ptr_(other.ptr_), ctrl_(other.ctrl_) {
        other.ptr_ = nullptr;
        other.ctrl_ = nullptr;
    }

    SharedPtr(const WeakPtr<T>& weak)
        : ptr_(nullptr), ctrl_(weak.ctrl_) {
        if (ctrl_ && ctrl_->strong_count.load(std::memory_order_acquire) > 0) {
            ctrl_->strong_count.fetch_add(1, std::memory_order_relaxed);
            ptr_ = ctrl_->ptr;
        } else {
            ctrl_ = nullptr;
        }
    }

    ~SharedPtr() {
        decrement_and_cleanup();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            decrement_and_cleanup();
            ptr_ = other.ptr_;
            ctrl_ = other.ctrl_;
            if (ctrl_) ctrl_->strong_count.fetch_add(1, std::memory_order_relaxed);
        }
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            decrement_and_cleanup();
            ptr_ = other.ptr_;
            ctrl_ = other.ctrl_;
            other.ptr_ = nullptr;
            other.ctrl_ = nullptr;
        }
        return *this;
    }

    void reset(T* ptr = nullptr) {
        decrement_and_cleanup();
        if (ptr) {
            ctrl_ = new ControlBlock<T>(ptr);
            ptr_ = ptr;
            ctrl_->strong_count.fetch_add(1, std::memory_order_relaxed);
        } else {
            ctrl_ = nullptr;
            ptr_ = nullptr;
        }
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(ctrl_, other.ctrl_);
    }

    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

    size_t use_count() const { return ctrl_ ? ctrl_->strong_count.load(std::memory_order_relaxed) : 0; }

private:
    void decrement_and_cleanup() {
        if (ctrl_) {
            if (ctrl_->strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete ctrl_->ptr;
                if (ctrl_->weak_count.load(std::memory_order_acquire) == 0) {
                    delete ctrl_;
                }
            }
            ctrl_ = nullptr;
            ptr_ = nullptr;
        }
    }

    T* ptr_;
    ControlBlock<T>* ctrl_;

    template<typename U, typename... Args>
    friend SharedPtr<U> make_shared(Args&&... args);
    friend class WeakPtr<T>;
};

// -------------------------
// weak_ptr
// -------------------------
template<typename T>
class WeakPtr {
public:
    WeakPtr() : ctrl_(nullptr) {}

    WeakPtr(const SharedPtr<T>& shared)
        : ctrl_(shared.ctrl_) {
        if (ctrl_) ctrl_->weak_count.fetch_add(1, std::memory_order_relaxed);
    }

    WeakPtr(const WeakPtr& other)
        : ctrl_(other.ctrl_) {
        if (ctrl_) ctrl_->weak_count.fetch_add(1, std::memory_order_relaxed);
    }

    WeakPtr(WeakPtr&& other) noexcept
        : ctrl_(other.ctrl_) {
        other.ctrl_ = nullptr;
    }

    ~WeakPtr() {
        release();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            release();
            ctrl_ = other.ctrl_;
            if (ctrl_) ctrl_->weak_count.fetch_add(1, std::memory_order_relaxed);
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            release();
            ctrl_ = other.ctrl_;
            other.ctrl_ = nullptr;
        }
        return *this;
    }

    SharedPtr<T> lock() const {
        if (ctrl_ && ctrl_->strong_count.load(std::memory_order_acquire) > 0) {
            return SharedPtr<T>(*this);
        }
        return SharedPtr<T>();
    }

    bool expired() const {
        return !ctrl_ || ctrl_->strong_count.load(std::memory_order_acquire) == 0;
    }

private:
    void release() {
        if (ctrl_) {
            if (ctrl_->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1 &&
                ctrl_->strong_count.load(std::memory_order_acquire) == 0) {
                delete ctrl_;
            }
            ctrl_ = nullptr;
        }
    }

    ControlBlock<T>* ctrl_;

    friend class SharedPtr<T>;
};

// -------------------------
// make_shared 实现
// -------------------------
template<typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    auto* ctrl = ControlBlock<T>::create(std::forward<Args>(args)...);
    SharedPtr<T> sp;
    sp.ptr_ = ctrl->ptr;
    sp.ctrl_ = ctrl;
    ctrl->strong_count.fetch_add(1, std::memory_order_relaxed);
    return sp;
}

} // namespace smart
