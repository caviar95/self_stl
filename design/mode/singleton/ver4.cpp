#include <iostream>

class Logger {
public:
    static Logger& GetInstance() {
        static Logger instance;

        return instance;
    }

    void Log(const std::string &message) {
        std::cout << message << std::endl;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() {}
};

int main()
{
    Logger &logger = Logger::GetInstance();

    logger.Log("THread-safe Singleton with static local variable");

    return 0;
}
