#pragma once

#include "channel.h"

class EventLoop;

class Acceptor {
public:
    Acceptor(EventLoop* loop, int port, std::function<void(int)> new_conn_cb);
    void listen();

private:
    int listen_fd_;
    Channel channel_;
    std::function<void(int)> new_conn_cb_;
};
