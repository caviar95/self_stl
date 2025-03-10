#include "scheduler.hpp"

#include "gtest/gtest.h"
#include <iostream>

TEST(Scheduler, BasicTest) {
    ThreadPool pool(4);
    Scheduler scheduler(pool);

    scheduler.ScheduleAfter(std::chrono::seconds(3), [] {
        std::cout << "task executed after 3 seconds\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));
}
