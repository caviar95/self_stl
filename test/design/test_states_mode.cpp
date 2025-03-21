#include "context.hpp"

#include "gtest/gtest.h"

TEST(States, StatePattern) {
    LeaveState initialState;
    // 通过构造函数注入初始状态
    Context context(&initialState);
    EXPECT_EQ(context.getCurrentStateName(), "Leaved");
    // std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    // 处理加入请求
    context.handleJoinRequest();
    EXPECT_EQ(context.getCurrentStateName(), "Joined");
    // std::cout << "after proc join, Current state: " << context.getCurrentStateName() << std::endl;

    // 处理加入请求（已加入）
    context.handleJoinRequest();
    EXPECT_EQ(context.getCurrentStateName(), "Joined");
    // std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    // 处理离开请求
    context.handleLeaveRequest();
    EXPECT_EQ(context.getCurrentStateName(), "Leaved");
    // std::cout << "Current state: " << context.getCurrentStateName() << std::endl;

    // 处理离开请求（已离开）
    context.handleLeaveRequest();
    EXPECT_EQ(context.getCurrentStateName(), "Leaved");
    // std::cout << "Current state: " << context.getCurrentStateName() << std::endl;
}