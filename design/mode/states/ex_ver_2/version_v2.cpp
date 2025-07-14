#include "fsm.hpp"

#include <iostream>

class StateMachineV2 : public IStateMachine {
public:
    StateMachineV2() {
        table_ = {
            {{State::Idle, Event::Start}, {State::Processing, [] {std::cout << "[V2] Start -> Processing\n"; }}},
            {{State::Processing, Event::Finish}, {State::Done, [] {std::cout << "[V2] Finish -> Done\n"; }}},
            {{State::Done, Event::Reset}, {State::Idle, [] {std::cout << "[V2] Reset -> Idle\n"; }}}
        };
    }

    void HandleEvent(State& current, Event evt) override {
        auto it = table_.find({current, evt});

        if (it != table_.end()) {
            it->second.action();
            PrintTransition(current, evt, it->second.nextState);
            current = it->second.nextState;
        } else {
            std::cout << "[V2] No transition for " << to_string(current) 
                      << " on event " << to_string(evt) << std::endl;
        }
    }

private:
    std::map<std::pair<State, Event>, Transition> table_;
};

std::unique_ptr<IStateMachine> CreateStateMachineV2() {
    return std::make_unique<StateMachineV2>();
}

