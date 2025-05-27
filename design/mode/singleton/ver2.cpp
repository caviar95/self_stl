#include <iostream>
#include <mutex>

class Logger {
public:
    static Logger *GetInstance() {
        std::lock_guard<std::mutex> lk(mutex_);

        if (instance == nullptr) {
            instance = new Logger();
        }

        return instance;
    }

    void Log(const std::string &message) {
        std::cout << message << std::endl;
    }

private:
    Logger() {}

    static Logger *instance;
    static std::mutex mutex_;
};

Logger *Logger::instance = nullptr;
std::mutex Logger::mutex_;

int main() {
    auto *instance = Logger::GetInstance();

    instance->Log("Hello ver2");

    return 0;
}
