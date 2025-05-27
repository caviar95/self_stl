#include <iostream>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>

// Shared Queue and Sync Mechanism
std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> done(false);

void producer() {
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock(mtx);
        buffer.push(i);
        std::cout << "Produced: " << i << std::endl;
        cv.notify_all();  // Notify consumers
    }
    done = true;
    cv.notify_all();  // Notify consumer to exit
}

void consumer() {
    while (!done || !buffer.empty()) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty() || done; });  // Wait for data to consume

        if (!buffer.empty()) {
            int data = buffer.front();
            buffer.pop();
            std::cout << "Consumed: " << data << std::endl;
        }
    }
}

int main() {
    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

    return 0;
}
