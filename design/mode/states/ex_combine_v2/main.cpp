#include <iostream>

#include "state_machine.h"

int main() {
    StateMachine sm;

    sm.AddMapping(StateMachine::STATE_A, "FINAL_X");
    sm.AddMapping(StateMachine::STATE_B, "FINAL_Y");
    sm.AddMapping(StateMachine::STATE_A | StateMachine::STATE_C, "FINAL_Z");
    sm.AddMapping(StateMachine::STATE_B | StateMachine::STATE_D, "FINAL_W");

    sm.PrintStateTable();

    uint32_t mask = StateMachine::STATE_A | StateMachine::STATE_C;
    std::cout << "Mask " << mask << " Final State: " << sm.FindFinalState(mask) << std::endl;

    uint32_t m1 = StateMachine::STATE_B | StateMachine::STATE_D;
    uint32_t m2 = StateMachine::STATE_B | StateMachine::STATE_D;

    std::cout << "Compare m1 & m2: " << (sm.CompareMasks(m1, m2) ? "Equal" : "Not Equal") << std::endl;

    return 0;
}
