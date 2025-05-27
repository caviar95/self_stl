#include <iostream>
#include <atomic>

class Logger {
public:
    static Logger *GetInstance() {
        Logger *tmp = instance.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            tmp = new Logger();

            if (instance.compare_exchange_strong(tmp, tmp)) {
                delete tmp;
                tmp = instance.load(std::memory_order_acquire);
            }
        }

        return tmp;
    }

    void Log(const std::string &message) {
        std::cout << message << std::endl;
    }

    Logger(const Logger&) = delete;
    Logger &operator=(const Logger&) = delete;

private:
    Logger() {}

    static std::atomic<Logger*> instance;
};

std::atomic<Logger*> Logger::instance = nullptr;

int main()
{
    Logger *logger = Logger::GetInstance();

    logger->Log("safe with atomic");

    return 0;
}
