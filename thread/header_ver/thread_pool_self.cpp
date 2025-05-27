#include "thread_pool_self.hpp"

#include <iostream>

int main()
{
    ThreadPool pool(4);

    std::vector<std::future<int>> futures;

    for (int i = 0; i < 8; ++i) {
        futures.emplace_back(pool.enqueue([i]() {
            return i * i;
        }));
    }

    for (auto &fut : futures) {
        std::cout << fut.get() << " ";
    }

    std::cout << std::endl;

    return 0;
}
