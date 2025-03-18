#include <iostream>
#include <memory>

// 抽象产品
class Logger {
public:
    virtual void log(const std::string& message) = 0;
    virtual ~Logger() = default;
};

// 具体产品
class FileLogger : public Logger {
public:
    void log(const std::string& message) override {
        std::cout << "File Log: " << message << std::endl;
    }
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& message) override {
        std::cout << "Console Log: " << message << std::endl;
    }
};

// 工厂类
class LoggerFactory {
public:
    enum LoggerType { FILE, CONSOLE };
    static std::unique_ptr<Logger> createLogger(LoggerType type) {
        switch (type) {
            case FILE:    return std::make_unique<FileLogger>();
            case CONSOLE: return std::make_unique<ConsoleLogger>();
            default:      throw std::invalid_argument("Invalid logger type");
        }
    }
};

int main() {
    auto logger = LoggerFactory::createLogger(LoggerFactory::FILE);
    logger->log("Hello Factory Pattern");
    return 0;
}