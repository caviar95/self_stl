#include "./thread_pool_11.hpp"

#include <iostream>
#include <vector>

int main() {
    ThreadPool pool(4);

    std::vector<std::future<int>> futures;
/*
    for (size_t i = 0; i < 8; ++i) {
        futures.emplace_back(pool.enqueue([i](){
            return i * i;
        }));        }
    }
*/

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
