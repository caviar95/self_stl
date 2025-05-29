#include <iostream>
#include <memory>

using namespace std;


template <typename T>
class UniquePtr {
public:
    explicit UniquePtr(T *ptr = nullptr) : ptr_(ptr) {}
    ~UniquePtr() {
        cout << __FUNCTION__ << endl;
        delete ptr_;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
        cout << __FUNCTION__ << endl;
        other.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr &&other) noexcept {
        cout << __FUNCTION__ << endl;
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }

        return *this;
    }

    T *get() const {
        return ptr_;
    }

    T *release() {
        T *tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T *ptr = nullptr) {
        delete ptr_;
        ptr_ = ptr;
    }

    T& operator*() {
        return *ptr_;
    }

    T* operator->() {
        return ptr_;
    }

private:
    T *ptr_;
};


void test() {
    int *a = new int();
    UniquePtr<int> up(a);

    UniquePtr<int> up2;
    up2 = move(up);
    cout << "after move up" << endl;
}

int main()
{

    test();

    return 0;
}