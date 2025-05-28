#include "event_loop.h"
#include "acceptor.h"
#include "connection_handler.h"

int main() {
    EventLoop loop;

    Acceptor acceptor(&loop, 12345, [&](int conn_fd) {
        new ConnectionHandler(&loop, conn_fd);
    });

    loop.loop();
    return 0;
}
