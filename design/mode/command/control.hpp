#pragma once

#include "command_intf.h"

class RemoteControl {
public:
    void SetCommand(CommandIntf* command) {
        this->command = command;
    }

    void Execute() {
        command->Execute();
    }

private:
    CommandIntf* command;
};
