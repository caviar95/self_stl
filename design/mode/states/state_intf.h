#pragma once

#include <functional>
#include <string>

class IState {
public:
    using StateChangeCallback = std::function<void(const std::string&)>;
    virtual void handleJoinRequest(const StateChangeCallback& callback) = 0;
    virtual void handleLeaveRequest(const StateChangeCallback& callback) = 0;
    virtual std::string getStateName() const noexcept = 0;
    virtual ~IState() = default;
};

