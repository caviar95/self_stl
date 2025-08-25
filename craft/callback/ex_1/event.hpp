#pragma once

#include <functional>

class EventSource {
public:
    ~EventSource() = default;
    
    void SetCallback(std::function<void(int)> cb) {
        cb_ = std::move(cb);
    }

    void Fire(int v) {
        if (cb_) {
            cb_(v);
        }
    }

private:
    std::function<void(int)> cb_;
};
