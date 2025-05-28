#include "connection_handler.h"
#include "event_loop.h"
#include <unistd.h>
#include <iostream>

ConnectionHandler::ConnectionHandler(EventLoop* loop, int fd)
    : conn_fd_(fd), channel_(fd) {
    channel_.setReadCallback([this]() {
        char buf[1024] = {0};
        ssize_t n = read(conn_fd_, buf, sizeof(buf));
        if (n > 0) {
            std::cout << "Recv: " << buf;
            write(conn_fd_, buf, n);  // echo
        } else if (n == 0) {
            close(conn_fd_);
        }
    });
    loop->updateChannel(&channel_);
}
