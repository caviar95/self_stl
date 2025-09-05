#include "mask_state_machine.hpp"

int main() {
    MaskStateMachine sm;
    sm.SetSubsystemState(0, 1);
    sm.SetSubsystemState(1, 3);
    sm.SetSubsystemState(2, 7);
    sm.Print();

    std::cout << "subsystem 1 state: " << sm.GetSusSystemState(1) << std::endl;

    sm.SetSubsystemState(0, 5);
    sm.Print();

    return 0;
}