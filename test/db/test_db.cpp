#include <string>

#include <string>

#include "gmock/gmock.h"

// 数据库客户端抽象接口
class DatabaseClient {
public:
    virtual ~DatabaseClient() = default;
    virtual bool connect(const std::string& url) = 0;
    virtual std::string query(const std::string& sql) = 0;
    virtual void disconnect() = 0;
};

class MockDatabaseClient : public DatabaseClient {
public:
    MOCK_METHOD(bool, connect, (const std::string& url), (override));
    MOCK_METHOD(std::string, query, (const std::string& sql), (override));
    MOCK_METHOD(void, disconnect, (), (override));
};

class UserService {
public:
    UserService(DatabaseClient& db) : db_(db) {}

    std::string getUserEmail(int userId) {
        if (!db_.connect("mysql://localhost:3306/users")) {
            throw std::runtime_error("Database connection failed");
        }

        std::string sql = "SELECT email FROM users WHERE id = " + std::to_string(userId);
        std::string result = db_.query(sql);
        db_.disconnect();
        
        if (result.empty()) return "not_found@example.com";
        return result;
    }

private:
    DatabaseClient& db_;
};

using ::testing::Return;
using ::testing::Throw;
using ::testing::HasSubstr;
using ::testing::_;

// 测试正常查询场景
TEST(UserServiceTest, ReturnsEmailWhenUserExists) {
    MockDatabaseClient mock_db;
    UserService service(mock_db);

    // 设置期望行为
    EXPECT_CALL(mock_db, connect("mysql://localhost:3306/users"))
        .WillOnce(Return(true));
    
    EXPECT_CALL(mock_db, query(HasSubstr("SELECT email")))
        .WillOnce(Return("john@example.com"));
    
    EXPECT_CALL(mock_db, disconnect());

    // 执行并验证
    ASSERT_EQ(service.getUserEmail(123), "john@example.com");
}

// 测试数据库连接失败
TEST(UserServiceTest, ThrowsExceptionOnConnectionFailure) {
    MockDatabaseClient mock_db;
    UserService service(mock_db);

    EXPECT_CALL(mock_db, connect(_))
        .WillOnce(Return(false));

    EXPECT_THROW(service.getUserEmail(123), std::runtime_error);
}

// 测试查询返回空结果
TEST(UserServiceTest, ReturnsDefaultEmailWhenUserNotFound) {
    MockDatabaseClient mock_db;
    UserService service(mock_db);

    EXPECT_CALL(mock_db, connect(_)).WillOnce(Return(true));
    EXPECT_CALL(mock_db, query(_)).WillOnce(Return(""));
    EXPECT_CALL(mock_db, disconnect());

    ASSERT_EQ(service.getUserEmail(456), "not_found@example.com");
}

// 验证 disconnect 是否总是被调用
TEST(UserServiceTest, AlwaysCallsDisconnect) {
    MockDatabaseClient mock_db;
    UserService service(mock_db);

    EXPECT_CALL(mock_db, connect(_))
        .WillOnce(Return(true));
    EXPECT_CALL(mock_db, query(_))
        .WillOnce(Return("test@example.com"));
    EXPECT_CALL(mock_db, disconnect())
        .Times(1); // 确保无论成功与否都调用

    service.getUserEmail(789);
}

