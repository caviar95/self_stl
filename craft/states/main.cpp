#include "my_state_machine.h"
#include <iostream>

std::string StateToStr(State s) {
    switch (s) {
        case State::Idle: return "Idle";
        case State::Running: return "Running";
        case State::Paused: return "Paused";
        case State::Stopped: return "Stopped";
        default: return "Unknown";
    }
}

int main() {
    MyStateMachine machine;

    auto printState = [&]() {
        std::cout << "Current State: " << StateToStr(machine.GetCurrentState()) << "\n";
    };

    printState();

    machine.HandleTrigger(Trigger::Start);
    printState();

    machine.HandleTrigger(Trigger::Pause);
    printState();

    machine.HandleTrigger(Trigger::Resume);
    printState();

    machine.HandleTrigger(Trigger::Stop);
    printState();

    machine.HandleTrigger(Trigger::Start);  // Restarting from stopped
    printState();

    machine.HandleTrigger(Trigger::Stop);
    printState();

    return 0;
}
