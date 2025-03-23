#pragma once

class CommandIntf {
public:
    virtual void Execute() = 0;
    virtual ~CommandIntf() = default;
};
