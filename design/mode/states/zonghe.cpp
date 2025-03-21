#include <iostream>
#include <string>
#include <functional>

// 抽象状态接口
class IState {
public:
    using StateChangeCallback = std::function<void(const std::string&)>;
    virtual void handleJoinRequest(const StateChangeCallback& callback) = 0;
    virtual void handleLeaveRequest(const StateChangeCallback& callback) = 0;
    virtual std::string getStateName() const = 0;
    virtual ~IState() = default;
};

// 上下文类
class Context {
private:
    IState* currentState;

    void changeState(const std::string& newStateName) {
        if (newStateName == "Joined") {
            static JoinedState joinedState;
            currentState = &joinedState;
        } else if (newStateName == "Left") {
            static LeftState leftState;
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

// 具体状态类：已加入状态
class JoinedState : public IState {
public:
    void handleJoinRequest(const StateChangeCallback& callback) override {
        std::cout << "Already joined. Ignoring join request." << std::endl;
    }

    void handleLeaveRequest(const StateChangeCallback& callback) override {
        std::cout << "Leaving the group." << std::endl;
        callback("Left");
    }

    std::string getStateName() const override {
        return "Joined";
    }
};

// 具体状态类：已离开状态
class LeftState : public IState {
public:
    void handleJoinRequest(const StateChangeCallback& callback) override {
        std::cout << "Joining the group." << std::endl;
        callback("Joined");
    }

    void handleLeaveRequest(const StateChangeCallback& callback) override {
        std::cout << "Already left. Ignoring leave request." << std::endl;
    }

    std::string getStateName() const override {
        return "Left";
    }
};

int main() {
    // 创建初始状态对象
    LeftState initialState;
    // 通过构造函数注入初始状态
    Context context(&initialState);

    std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    // 处理加入请求
    context.handleJoinRequest();
    std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    // 处理加入请求（已加入）
    context.handleJoinRequest();
    std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    // 处理离开请求
    context.handleLeaveRequest();
    std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    // 处理离开请求（已离开）
    context.handleLeaveRequest();
    std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    return 0;
}    