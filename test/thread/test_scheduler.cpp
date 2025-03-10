#include "scheduler.hpp"

#include "gtest/gtest.h"
#include <iostream>

TEST(Scheduler, BasicTest) {
    ThreadPool pool(4);
    Scheduler scheduler(pool);

    // scheduler.ScheduleAfter(std::chrono::seconds(3), [] {
    //     std::cout << "task executed after 3 seconds\n";
    // });

    scheduler.Schedule(std::chrono::seconds(3), [] {
        std::cout << "single shot task\n";
    });


    scheduler.Schedule(std::chrono::seconds(1), [] {
        std::cout << "Infinite loop task\n";
    }, -1);

    scheduler.Schedule(std::chrono::seconds(2), [] {
        static int count = 0;
        std::cout << "Limited task: " << ++count << "\n";
    }, 3);

    std::this_thread::sleep_for(std::chrono::seconds(10));
}
