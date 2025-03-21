#pragma once

#include <iostream>

#include "state_intf.h"

// 具体状态类：已加入状态
class JoinedState : public IState {
public:
    void handleJoinRequest(const StateChangeCallback& callback) override {
        std::cout << "Already joined. Ignoring join request." << std::endl;
    }

    void handleLeaveRequest(const StateChangeCallback& callback) override {
        std::cout << "Leaving the group." << std::endl;
        callback("Leaved");
    }

    std::string getStateName() const noexcept override {
        static constexpr const char* name = "Joined";
        return name;
    }
};
