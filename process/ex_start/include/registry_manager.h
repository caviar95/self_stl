#pragma once
#include <string>

struct ServiceInfo {
    std::string service_name;
    int port = 0;
    pid_t pid = 0;
};

class RegistryManager {
public:
    void registerService(const ServiceInfo& info);
};
