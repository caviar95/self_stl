#include "logger.hpp"

void worker(int id) {
    for (int i = 0; i < 3; ++i) {
        LOG_INFO("Worker %d - iteration %d", id, i);
    }
}

int main() {
    LOG_INFO("Starting Logger test");

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    t1.join();
    t2.join();

    LOG_WARN("Test Completed");

    return 0;
}