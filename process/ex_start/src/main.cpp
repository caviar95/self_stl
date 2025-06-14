#include "process_manager.h"
#include <iostream>
#include <thread>

int main() {
    ProcessManager pm;
    if (!pm.loadConfig("config/processes.json")) {
        std::cerr << "Failed to load config.\n";
        return 1;
    }

    pm.startAll();
    pm.monitor();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}
