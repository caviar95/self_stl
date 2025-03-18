#pragma once

#include <future>

class OneShotEvent {
    std::promise<void> promise;
    std::future<void> future;
public:
    OneShotEvent() : future(promise.get_future()) {}
    void wait() { future.wait(); }
    void signal() { promise.set_value(); }
};
