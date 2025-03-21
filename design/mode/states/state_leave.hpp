#pragma once

#include <iostream>

#include "state_intf.h"

// 具体状态类：已离开状态
class LeaveState : public IState {
public:
    void handleJoinRequest(const StateChangeCallback& callback) override {
        std::cout << "Joining the group." << std::endl;
        callback("Joined");
    }

    void handleLeaveRequest(const StateChangeCallback& callback) override {
        std::cout << "Already leaved. Ignoring leave request." << std::endl;
    }

    std::string getStateName() const noexcept override {
        static constexpr const char* name = "Leaved";
        return name;
    }
};
