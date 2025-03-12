#pragma once

#include <iostream>
#include <string>

// 示例：数据库连接对象
class DatabaseConnection {
public:
    std::string connectionId;
    bool isConnected;

    // 构造函数（模拟资源初始化）
    DatabaseConnection(const std::string& id) : connectionId(id), isConnected(true) {
        std::cout << "Connection " << connectionId << " established.\n";
    }

    // 重置方法：将对象恢复到可复用状态
    void reset(const std::string& newId) {
        if (isConnected) {
            close(); // 关闭当前连接
        }
        connectionId = newId;
        connect();  // 重新连接
    }

    // 模拟资源释放
    void close() {
        isConnected = false;
        std::cout << "Connection " << connectionId << " closed.\n";
    }

private:
    void connect() {
        isConnected = true;
        std::cout << "Connection " << connectionId << " reopened.\n";
    }
};
