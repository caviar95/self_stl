#pragma once

#include "state.hpp"
#include <functional>
#include <map>
#include <memory>

struct Transition {
    State nextState;
    std::function<void()> action;
};

class IStateMachine {
public:
    virtual void HandleEvent(State& current, Event event) = 0;
    virtual ~IStateMachine() = default;
};

void PrintTransition(State from, Event evt, State to);
