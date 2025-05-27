#include "scheduler.hpp"

int main() {
    ThreadPool pool(3);           // 3个线程
    Scheduler scheduler(pool, 1); // 每秒提交任务

    scheduler.start();
    std::this_thread::sleep_for(std::chrono::seconds(8));
    scheduler.stopScheduler();

    std::cout << "Main exiting.\n";
    return 0;
}
