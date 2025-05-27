#include "dynamic_thread_pool.hpp"

#include <iostream>
#include <chrono>
#include <sstream>

void safePrint(const std::string &msg) {
    static std::mutex mtx;

    std::lock_guard<std::mutex> lk(mtx);

    std::cout << msg << std::endl;
}

int main() {
    DynamicThreadPool pool(2, 6);

    for (int i = 0; i < 20; ++i) {
        pool.submit([i]() {
            std::ostringstream oss{};
            
            oss << "task " <<  i << " is executed by " << std::this_thread::get_id();
            // std::cout << "task " << i << " is executed by " << std::this_thread::get_id() << std::endl;
            safePrint(oss.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(8));

    std::cout << "main procedure" << std::endl;

    return 0;
}

