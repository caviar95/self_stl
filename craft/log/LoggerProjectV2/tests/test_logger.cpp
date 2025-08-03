#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/ILogger.h"

class MockLogger : public ILogger {
public:
    MOCK_METHOD(void, LogRaw, (int mode, const std::string& message), (override));
};

TEST(LoggerTest, RawLogging) {
    MockLogger mock;
    EXPECT_CALL(mock, LogRaw(1, testing::HasSubstr("Test message")));
    mock.LogRaw(1, "Test message from logger");
}
