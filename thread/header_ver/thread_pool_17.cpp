#include "./thread_pool_17.hpp"

#include <iostream>
#include <chrono>

int main()
{
    ThreadPool pool(4);

    std::vector<std::future<int>> results;

    for (int i = 0; i < 8; ++i) {
        results.emplace_back(pool.enqueue([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return i * i;
        }));
    }

    for (auto &fut : results) {
        std::cout << fut.get() << " ";
    }

    std::cout << std::endl;

    return 0;
}
