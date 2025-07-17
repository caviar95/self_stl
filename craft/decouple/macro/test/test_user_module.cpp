#include <gtest/gtest.h>
#include "../logging/mock_adapter.h"
#include "../user/user_module.h"

TEST(UserModuleTest, LogVectorIsCalled) {
    MockLoggingAdapter mock;
    gLogger = &mock;

    EXPECT_CALL(mock, LogVector(1, 0x1234, testing::ElementsAre("42")));
    EXPECT_CALL(mock, LogVector(2, 0x2345, testing::ElementsAre("example", "test")));

    UserModuleFunction();
}
