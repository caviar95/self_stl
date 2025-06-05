#include "../include/command_registry.h"

#include <iostream>

int main() {
    CommandRegistry registry;

    registry.RegisterCommand({"show", "info"}, [](const std::unordered_map<std::string, std::string>& opts) {
        std::cout << "[Handler] show info\n";

        for (auto& [k, v] : opts) {
            std::cout << " - " << k << ": " << v << "\n";
        }
    });

    registry.RegisterCommand({"set", "config"}, [](const std::unordered_map<std::string, std::string>& opts) {
        std::cout << "[Handler] set config\n";
    });

    registry.Dispatch("show info port 8080");

    registry.Dispatch("set config name server1");

    return 0;
}
