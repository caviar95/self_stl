#pragma once

#include <chrono>

using namespace std::chrono;

class TimerBase {
public:
    TimerBase() : start_(system_clock::time_point::min()) {}

    void Clear() {
        start_ = system_clock::time_point::min();
    }

    bool IsStarted() const {
        return (start_.time_since_epoch() != system_clock::duration(0));
    }

    void Start() {
        start_ = system_clock::now();
    }

    unsigned long GetMs() {
        if (IsStarted()) {
            system_clock::duration diff = system_clock::now() - start_;
            return (unsigned long)(duration_cast<milliseconds>(diff).count());
        }

        return 0;
    }

private:
    system_clock::time_point start_;
};
