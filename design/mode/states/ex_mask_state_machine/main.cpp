#include "mask_state_machine.hpp"
#include <iostream>

int main() {
    MaskStateMachine sm;

    if (sm.CanTransitionTo(0)) {
        sm.SetState(0);
    }

    if (sm.CanTransitionTo(1)) {
        sm.SetState(1);
    }

    if (sm.CanTransitionTo(2)) {
        sm.SetState(2);
    }

    std::cout << "State: " << std::hex << sm.GetState() << std::endl;

    sm.ClearState(1);

    std::cout << "State after clear bit 1: " << std::hex << sm.GetState() << std::endl;

    return 0;
}