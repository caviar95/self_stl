#include "process_manager.h"
// #include <nlohmann/json.hpp>
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <thread>
#include <chrono>

using json = nlohmann::json;

bool ProcessManager::loadConfig(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    json j;
    in >> j;
    for (const auto& item : j["processes"]) {
        ProcessConfig cfg;
        cfg.name = item["name"];
        cfg.exec = item["exec"];
        for (const auto& arg : item["args"]) {
            cfg.args.push_back(arg);
        }
        cfg.work_dir = item["work_dir"];
        cfg.restart = item["restart"];
        if (item.contains("register")) {
            cfg.service_info.service_name = item["register"]["service_name"];
            cfg.service_info.port = item["register"]["port"];
        }
        configs_.push_back(cfg);
    }
    return true;
}

pid_t ProcessManager::startProcess(const ProcessConfig& config) {
    pid_t pid = fork();
    if (pid == 0) {
        chdir(config.work_dir.c_str());
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(config.exec.c_str()));
        for (const auto& arg : config.args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        execv(config.exec.c_str(), argv.data());
        perror("execv failed");
        exit(1);
    }

    ProcessConfig running_cfg = config;
    running_cfg.service_info.pid = pid;
    RegistryManager reg;
    if (!config.service_info.service_name.empty())
        reg.registerService(running_cfg.service_info);

    running_processes_[pid] = config;
    std::cout << "[Launcher] Started process '" << config.name << "' (PID: " << pid << ")\n";
    return pid;
}

void ProcessManager::startAll() {
    for (const auto& config : configs_) {
        startProcess(config);
    }
}

void ProcessManager::monitor() {
    std::thread([this]() {
        while (true) {
            int status = 0;
            pid_t pid = waitpid(-1, &status, WNOHANG);
            if (pid > 0) {
                std::cout << "[Monitor] Process " << pid << " exited.\n";
                auto it = running_processes_.find(pid);
                if (it != running_processes_.end()) {
                    if (it->second.restart) {
                        std::cout << "[Monitor] Restarting " << it->second.name << "...\n";
                        startProcess(it->second);
                    }
                    running_processes_.erase(it);
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();
}
