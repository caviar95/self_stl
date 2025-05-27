#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void worker() {
    std::unique_lock<std::mutex> lock(mtx);
    std::cout << "Worker: waiting...\n";
    
    // Running → Waiting（挂起等待条件变量）
    cv.wait(lock, [] { return ready; });

    std::cout << "Worker: resumed and working...\n";
    // Running → Sleeping
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Worker: finished.\n";
    // → Terminated
}

int main() {
    std::thread t(worker);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
        std::cout << "Main: notifying worker...\n";
    }
    cv.notify_one();

    t.join();
}
