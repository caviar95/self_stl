#include <iostream>

class Logger {
public:
    static Logger *GetInstance() {
        if (instance == nullptr) {
            instance = new Logger();
        }

        return instance;
    }

    void Log(const std::string &message) {
        std::cout << message << std::endl;
    }

private:
    static Logger *instance;

    Logger() {}
};

Logger *Logger::instance = nullptr;

int main()
{
    Logger *logger = Logger::GetInstance();

    logger->Log("Hello, Singletone!");

    return 0;
}
