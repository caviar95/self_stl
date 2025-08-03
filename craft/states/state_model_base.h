#pragma once
#include <map>
#include <functional>
#include <utility>
#include <iostream>

// 定义枚举状态和触发器
enum class State {
    Idle,
    Running,
    Paused,
    Stopped
};

enum class Trigger {
    Start,
    Pause,
    Resume,
    Stop
};

// 状态转换结构体
struct TransType {
    State cur;
    Trigger trig;
    State next;
    std::function<void()> action;
    std::function<bool()> canUpdateState;
};

// 状态机基类
class StateModelBase {
public:
    StateModelBase(State initialState) : currentState(initialState) {}

    void RegisterTransition(State cur, Trigger trig, State next,
                            std::function<void()> action,
                            std::function<bool()> canUpdateState = nullptr) {
        transitionTable[{cur, trig}] = TransType{cur, trig, next, action, canUpdateState};
    }

    bool HandleTrigger(Trigger trig) {
        auto key = std::make_pair(currentState, trig);
        if (transitionTable.count(key)) {
            const auto& trans = transitionTable[key];
            if (!trans.canUpdateState || trans.canUpdateState()) {
                if (trans.action) trans.action();
                currentState = trans.next;
                return true;
            }
        } else {
            std::cout << "Invalid transition from state "
                      << static_cast<int>(currentState)
                      << " using trigger " << static_cast<int>(trig) << "\n";
        }
        return false;
    }

    State GetCurrentState() const { return currentState; }

protected:
    using StateKey = std::pair<State, Trigger>;
    std::map<StateKey, TransType> transitionTable;
    State currentState;
};
