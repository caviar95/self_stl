#include <iostream>
#include <mutex>

class Logger {
public:
    static Logger &GetInstance() {
        static Logger instance;

        return instance;
    }

    void Log(const std::string &message) {
        std::lock_guard<std::mutex> lk(mtx);

        std::cout << message << std::endl;
    }


    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() {}
    std::mutex mtx;
};

int main() {
    Logger &logger = Logger::GetInstance();

    logger.Log("Thread-safe singleton with initialization and oper");

    return 0;
}
