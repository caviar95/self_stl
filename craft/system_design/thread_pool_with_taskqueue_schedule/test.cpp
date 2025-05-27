#include "scheduler.hpp"

int main() {
    ThreadPool pool(4);           // 启动 4 个工作线程
    Scheduler scheduler(pool, 1); // 每秒投递一个任务

    scheduler.start();

    std::this_thread::sleep_for(std::chrono::seconds(6)); // 主线程运行 6 秒
    scheduler.stopScheduler(); // 停止调度器
    std::cout << "Main exiting.\n";

    return 0; // 线程池析构自动 join 所有线程
}