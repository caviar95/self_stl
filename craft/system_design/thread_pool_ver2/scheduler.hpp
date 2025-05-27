#pragma once

#include "thread_pool.hpp"

class Scheduler {
public:
    Scheduler(ThreadPool &pool, int interval_sec = 1)
        : pool(pool), interval(interval_sec), running(true) {}

    void start() {
        schedulerThread = std::thread([this] {
            int counter = 0;
            while (running) {
                std::this_thread::sleep_for(std::chrono::seconds(interval));
                int prio = (counter % 2 == 0) ? 1 : 10;

                pool.enqueue([counter, prio]() {
                    std::cout << "[Task] Executing task #" << counter
                              << " (priority " << prio << ")\n";
                }, prio);

                counter++;
            }
        });
    }

    void stopScheduler() {
        running = false;
        if (schedulerThread.joinable()) schedulerThread.join();
    }

    ~Scheduler() {
        stopScheduler();
    }

private:
    ThreadPool &pool;
    std::thread schedulerThread;
    int interval;
    std::atomic<bool> running;
};