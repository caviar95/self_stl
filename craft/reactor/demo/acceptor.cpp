#include "acceptor.h"
#include "event_loop.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

static int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Acceptor::Acceptor(EventLoop* loop, int port, std::function<void(int)> cb)
    : listen_fd_(socket(AF_INET, SOCK_STREAM, 0)),
      channel_(listen_fd_),
      new_conn_cb_(cb) {

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(listen_fd_, (sockaddr*)&addr, sizeof(addr));
    setNonBlocking(listen_fd_);
    ::listen(listen_fd_, SOMAXCONN);

    channel_.setReadCallback([this]() {
        sockaddr_in cli_addr;
        socklen_t len = sizeof(cli_addr);
        int conn_fd = accept(listen_fd_, (sockaddr*)&cli_addr, &len);
        if (conn_fd >= 0) {
            setNonBlocking(conn_fd);
            new_conn_cb_(conn_fd);
        }
    });

    loop->updateChannel(&channel_);
}
