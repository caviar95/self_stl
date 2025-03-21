#pragma once

#include "state_join.hpp"
#include "state_leave.hpp"

// 上下文类
class Context {
private:
    IState* currentState;

    void changeState(const std::string& newStateName) {
        if (newStateName == "Joined") {
            static JoinedState joinedState;
            currentState = &joinedState;
        } else if (newStateName == "Leaved") {
            static LeaveState leftState;
            currentState = &leftState;
        }
    }

public:
    // 通过构造函数注入初始状态
    Context(IState* initialState) : currentState(initialState) {}

    void handleJoinRequest() {
        currentState->handleJoinRequest([this](const std::string& newStateName) {
            changeState(newStateName);
        });
    }

    void handleLeaveRequest() {
        currentState->handleLeaveRequest([this](const std::string& newStateName) {
            changeState(newStateName);
        });
    }

    std::string getCurrentStateName() const {
        return currentState->getStateName();
    }
};
