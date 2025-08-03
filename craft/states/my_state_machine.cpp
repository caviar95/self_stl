#include "my_state_machine.h"

MyStateMachine::MyStateMachine() : StateModelBase(State::Idle) {
    RegisterTransition(State::Idle, Trigger::Start, State::Running,
        []() { std::cout << "[Action] Starting...\n"; });

    RegisterTransition(State::Running, Trigger::Pause, State::Paused,
        []() { std::cout << "[Action] Pausing...\n"; });

    RegisterTransition(State::Paused, Trigger::Resume, State::Running,
        []() { std::cout << "[Action] Resuming...\n"; });

    RegisterTransition(State::Running, Trigger::Stop, State::Stopped,
        []() { std::cout << "[Action] Stopping from Running...\n"; });

    RegisterTransition(State::Paused, Trigger::Stop, State::Stopped,
        []() { std::cout << "[Action] Stopping from Paused...\n"; });

    RegisterTransition(State::Stopped, Trigger::Start, State::Running,
        []() { std::cout << "[Action] Restarting from Stopped...\n"; },
        []() {
            std::cout << "[Guard] Checking if restart is allowed...\n";
            return true;
        });
}
