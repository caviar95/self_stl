#include "logger.hpp"
#include "log_worker.hpp"
#include "log_config.h"

#include <thread>

void worker(int id) {
    for (int i = 0; i < 3; ++i) {
        LOG_INFO("Worker %d - iteration %d", id, i);
    }
}

int main() {
    auto& config = LogConfig::get();
    config.setStrategy(LogDropStrategy::Block);   // 选 Drop / Block / Yield
    config.setConsoleOutput(false);               // false = 写文件
    config.setLogPath("app.log");

    LogWorker::GetInstance().start();



    std::thread t1([] {
        for (int i = 0; i < 10000; ++i) {
            LOG_INFO("Thread 1 log: %d", i);
        }
    });

    std::thread t2([] {
        for (int i = 0; i < 10000; ++i) {
            LOG_INFO("Thread 2 log: %d", i);
        }
    });

    t1.join();
    t2.join();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    LogWorker::GetInstance().stop();
    return 0;
}