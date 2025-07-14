#include "fsm.hpp"

#include <iostream>

class StateMachineV1 : public IStateMachine {
public:
    StateMachineV1() {
        table_ = {
            {{State::Idle, Event::Start}, {State::Done, [] {std::cout << "[V1] Start -> Done\n"; }}},
            {{State::Done, Event::Reset}, {State::Idle, [] {std::cout << "[V1] Reset -> Idle\n"; }}} 

        };
    }

    void HandleEvent(State& current, Event evt) override {
        auto it = table_.find({current, evt});

        if (it != table_.end()) {
            it->second.action();
            PrintTransition(current, evt, it->second.nextState);
            current = it->second.nextState;
        } else {
            std::cout << "[V1] No transition for " << to_string(current) 
                      << " on event " << to_string(evt) << std::endl;
        }
    }

private:
    std::map<std::pair<State, Event>, Transition> table_;
};

std::unique_ptr<IStateMachine> CreateStateMachineV1() {
    return std::make_unique<StateMachineV1>();
}
