#include <iostream>
#include <vector>
#include <functional>

class EventDispatcher {
    std::vector<std::function<void(int)>> callbacks;

public:
    void registerCallback(std::function<void(int)> cb) {
        callbacks.push_back(cb);
    }

    void dispatch(int eventData) {
        for (auto& cb : callbacks) {
            cb(eventData);
        }
    }
};

int main() {
    EventDispatcher dispatcher;

    // 注册多个回调
    dispatcher.registerCallback([](int data) {
        std::cout << "Callback1 received " << data << "\n";
    });

    int base = 100;
    dispatcher.registerCallback([base](int data) {
        std::cout << "Callback2 processed " << data + base << "\n";
    });

    // 触发事件
    dispatcher.dispatch(5);

    return 0;
}