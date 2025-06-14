#pragma once
#include <string>
#include <vector>
#include <map>
#include "registry_manager.h"

struct ProcessConfig {
    std::string name;
    std::string exec;
    std::vector<std::string> args;
    std::string work_dir;
    bool restart;
    ServiceInfo service_info;
};

class ProcessManager {
public:
    bool loadConfig(const std::string& path);
    void startAll();
    void monitor(); // 启动后台线程监控进程

private:
    std::vector<ProcessConfig> configs_;
    std::map<pid_t, ProcessConfig> running_processes_;

    pid_t startProcess(const ProcessConfig& config);
};
