#pragma once

#include "channel.h"
#include <memory>

class EventLoop;

class ConnectionHandler {
public:
    ConnectionHandler(EventLoop* loop, int fd);

private:
    int conn_fd_;
    Channel channel_;
};
