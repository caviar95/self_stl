#include "channel.h"
#include <sys/epoll.h>

Channel::Channel(int fd)
    : fd_(fd), events_(EPOLLIN) {}

void Channel::setReadCallback(Callback cb) {
    read_callback_ = std::move(cb);
}

void Channel::handleEvent() {
    if (read_callback_) read_callback_();
}

int Channel::fd() const { return fd_; }
int Channel::events() const { return events_; }
