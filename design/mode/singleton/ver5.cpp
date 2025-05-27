#include <iostream>

class Logger {
public:
    static Logger &GetInstance() {
        thread_local static Logger instance;
        return instance;
    }

    void Log(const std::string & message) {
        std::cout << message << std::endl;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:

    Logger() {}
};

int main() {
    Logger &logger = Logger::GetInstance();

    logger.Log("Thread-safe ver2");

    return 0;
}
