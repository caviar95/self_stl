#pragma once

template <typename T>
class CommandHandler {
public:
    virtual void handle(const T& command) = 0;
    virtual ~CommandHandler() = default;
};

