#include "registry_manager.h"
#include <iostream>

void RegistryManager::registerService(const ServiceInfo& info) {
    std::cout << "[Registry] Service '" << info.service_name
              << "' running on port " << info.port
              << " (PID: " << info.pid << ")\n";
}
