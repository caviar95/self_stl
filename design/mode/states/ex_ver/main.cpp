#include "version.hpp"
#include <iostream>

int main() {
    Version version = Version::V2; // Change to V1 or V2 as needed
    auto stateMachine = CreateStateMachine(version);

    State currentState = State::Idle;
    std::cout << "Initial State: " << to_string(currentState) << std::endl;

    stateMachine->HandleEvent(currentState, Event::Start);
    stateMachine->HandleEvent(currentState, Event::Finish);
    stateMachine->HandleEvent(currentState, Event::Reset);
    stateMachine->HandleEvent(currentState, Event::Start);
    stateMachine->HandleEvent(currentState, Event::Finish);

    return 0;
}