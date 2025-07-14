#include "registry.hpp"
#include "logger.hpp"

AutoRegister<ILogger, ProductLogger> auto_reg_logger;

class Singleton {
public:
    Singleton() {
        auto ptr = AutoRegistry<ILogger>::CreateDefault();
        logger_ = ptr.release();  // raw pointer for simplicity
    }

    ~Singleton() {
        delete logger_;
    }

    void DoSomething() {
        if (logger_) {
            logger_->Log("Singleton is doing something...");
        }
    }

private:
    ILogger* logger_;
};

int main() {
    Singleton instance;
    instance.DoSomething();
    return 0;
}
