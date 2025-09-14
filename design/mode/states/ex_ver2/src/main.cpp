#include "state_machine_derived.h"
#include <iostream>

int main() {
    MyStateMachine sm;
    sm.onFinalChanged([] (const std::string& name) {
        std::cout << "Final changed to: " << name << std::endl;
    });

    FinalState<2> fs;
    fs.name = "AllIdle";
    fs.required.bits[0].set(0);
    fs.required.bits[1].set(0);
    sm.addFinalState(fs);

    sm.setSubsystemState(0, 0);
    sm.setSubsystemState(1, 0);

    return 0;
}
