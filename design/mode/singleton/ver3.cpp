#include <iostream>

class Logger {
public:
    static Logger &GetInstance() {
        return instance;
    }

    void Log(const std::string &message) {
        std::cout << message << std::endl;
    }
    
private:
    static Logger instance;
    Logger() {
    }
};

Logger Logger::instance;

int main()
{
    Logger &logger = Logger::GetInstance();

    logger.Log("Eager Initialization Singleton");

    return 0;
}
