#pragma once

class Command {
public:
    virtual ~Command() = default;
    virtual void printInfo() const = 0;
};
