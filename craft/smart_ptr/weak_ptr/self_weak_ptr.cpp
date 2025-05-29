
template <typename T>
class WeakPtr {
public:
    WeakPtr() : ptr_(nullptr), ctrl_(nullptr) {}

    WeakPtr(const SharedPtr& sp) : ptr_(sp.ptr_), ctrl_(sp.ctrl_) {
        if (ctrl_) {
            ++ctrl_->weak_count;
        }
    }

    ~WeakPtr() {
        if (ctrl_ && --ctrl_->weak_count == 0 && ctrl_->strong_count == 0) {
            delete ctrl_;
        }
    }

    SharedPtr<T> lock() const {
        if (ctrl_ && ctrl_->strong_count > 0) {
            return SharedPtr<T>(*this);
        }

        return SharedPtr<T>();
    }

private:
    T* ptr_;
    ControlBlock* ctrl_;

    friend class SharedPtr<T>;

    WeakPtr(T* ptr, ControlBlock *ctrl) : ptr_(ptr), ctrl_(ctrl) {}
};