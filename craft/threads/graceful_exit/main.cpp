#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

std::atomic<bool> exitFlag = false;

void worker(int id) {
    while (!exitFlag.load()) {
        std::cout <<  "thread: " << id << " is working..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(worker, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "notify all threads exit..." << std::endl;
    exitFlag = true;

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "all threads exit, program end" << std::endl;
    return 0;
}