#include <iostream>
#include <cstddef>
#include <memory>

struct ControlBlock {
    size_t strong_count = 1;
    size_t weak_count = 0;
};

template <typename T>
class SharedPtr {
public:
    SharedPtr() : ptr_(nullptr), ctrl_(nullptr) {}

    explicit SharedPtr(T* ptr) : ptr_(ptr) {
        ctrl_ = new ControlBlock();
    }

    SharedPtr(const SharedPtr &other) : ptr_(other.ptr_), ctrl_(other.ctrl_) {
        if (ctrl_) {
            ++ctrl_->strong_count;
        }
    }

    ~SharedPtr() {
        release();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            ptr_ = other.ptr_;
            ctrl_ = other.ctrl_;
            if (ctrl_) {
                ++ctrl_->strong_count;
            }
        }

        return *this;
    }

    size_t use_count() const {
        return ctrl_ != nullptr ? ctrl_->strong_count : 0;
    }

    T* get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }
private:
    T *ptr_;
    ControlBlock *ctrl_;

    void release() {
        if (ctrl_ != nullptr && --ctrl_->strong_count == 0) {
            delete ptr_;
            if (ctrl_->weak_count == 0) {
                delete ctrl_;
            }
        }
    }

    template <typename U>
    friend class WeakPtr;
};

void test() {
    SharedPtr<int> a(new int(5));
    std::cout << a.use_count() << std::endl;
    SharedPtr<int> b = a;
    std::cout << a.use_count() << std::endl;
    std::cout << b.use_count() << std::endl;
}

int main() {
    test();
    return 0;
}