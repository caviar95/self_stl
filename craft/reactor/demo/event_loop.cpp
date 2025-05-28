#include "event_loop.h"
#include "channel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

EventLoop::EventLoop() {
    epoll_fd_ = epoll_create1(0);
}

EventLoop::~EventLoop() {
    close(epoll_fd_);
}

void EventLoop::updateChannel(Channel* channel) {
    epoll_event ev;
    ev.events = channel->events();
    ev.data.ptr = channel;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &ev) == -1) {
        std::cerr << "epoll_ctl failed\n";
    }
}

void EventLoop::loop() {
    epoll_event events[MAX_EVENTS];
    while (true) {
        int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i) {
            Channel* ch = static_cast<Channel*>(events[i].data.ptr);
            ch->handleEvent();
        }
    }
}
