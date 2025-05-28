#pragma once

#include <vector>
#include <functional>

class Channel;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void updateChannel(Channel* channel);

private:
    int epoll_fd_;
    static constexpr int MAX_EVENTS = 1024;
};
