#pragma once

#include <functional>

class Channel {
public:
    using Callback = std::function<void()>;

    Channel(int fd);
    void setReadCallback(Callback cb);
    void handleEvent();
    int fd() const;
    int events() const;

private:
    int fd_;
    int events_;
    Callback read_callback_;
};
