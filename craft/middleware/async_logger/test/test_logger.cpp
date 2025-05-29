#include "logger.hpp"
#include "log_worker.hpp"

#include <thread>

void worker(int id) {
    for (int i = 0; i < 3; ++i) {
        LOG_INFO("Worker %d - iteration %d", id, i);
    }
}

int main() {
    // LOG_INFO("Starting Logger test");

    // std::thread t1(worker, 1);
    // std::thread t2(worker, 2);

    // t1.join();
    // t2.join();

    // LOG_WARN("Test Completed");

    LogWorker::GetInstance().start();

    std::thread t1([]{
        for (int i = 0; i < 1000; ++i) {
            LOG_INFO("Thread 1 - log %d", i);
        }
    });

    std::thread t2([] {
        for (int i = 0; i < 1000; ++i) {
            LOG_INFO("Thread 2 - log %d", i);
        }
    });

    t1.join();
    t2.join();

    LOG_WARN("Logging finished.");

    std::this_thread::sleep_for(std::chrono::seconds(1));

    LogWorker::GetInstance().stop();

    return 0;
}