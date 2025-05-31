#include "logger.hpp"
#include "log_worker.hpp"
#include "log_config.h"

#include <thread>

// void worker(int id) {
//     for (int i = 0; i < 3; ++i) {
//         LOG_INFO("Worker %d - iteration %d", id, i);
//     }
// }

int main() {
    auto& cfg = LogConfig::GetInstance();
    cfg.SetConsoleOutput(false);
    cfg.SetLogPath("output.log");
    cfg.SetStrategy(LogDropStrategy::Yield);
    cfg.SetLogLevel(LogLevel::INFO);

    Logger::GetInstance().Init(LogLevel::INFO);

    std::thread t1([] {
        for (int i = 0; i < 1000; ++i)
            Logger::GetInstance().Log(LogLevel::INFO, "T1", "Logging from thread 1");
    });

    std::thread t2([] {
        for (int i = 0; i < 1000; ++i)
            Logger::GetInstance().Log(LogLevel::INFO, "T2", "Logging from thread 2");
    });

    t1.join();
    t2.join();

    Logger::GetInstance().Stop();

    return 0;
}