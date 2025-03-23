#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

class LogRotator {
public:
    LogRotator(const std::string& logFileName, size_t maxSize, int maxFiles)
        : logFileName(logFileName), maxSize(maxSize), maxFiles(maxFiles) {
        openLogFile();
    }

    ~LogRotator() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const std::string& message) {
        if (logFile.is_open()) {
            logFile << getCurrentTime() << " - " << message << std::endl;
            if (logFile.tellp() >= static_cast<std::streamsize>(maxSize)) {
                rotateLogs();
            }
        }
    }

private:
    std::string logFileName;
    size_t maxSize;
    int maxFiles;
    std::ofstream logFile;

    void openLogFile() {
        logFile.open(logFileName, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFileName << std::endl;
        }
    }

    std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        char* dt = std::ctime(&now);
        std::string timeStr(dt);
        // Remove the newline character at the end
        if (!timeStr.empty() && timeStr[timeStr.length() - 1] == '\n') {
            timeStr.erase(timeStr.length() - 1);
        }
        return timeStr;
    }

    void rotateLogs() {
        logFile.close();
        for (int i = maxFiles - 1; i > 0; --i) {
            std::string oldLog = logFileName + "." + std::to_string(i);
            std::string newLog = logFileName + "." + std::to_string(i + 1);
            if (fs::exists(oldLog)) {
                if (fs::exists(newLog)) {
                    fs::remove(newLog);
                }
                fs::rename(oldLog, newLog);
            }
        }
        std::string backupLog = logFileName + ".1";
        if (fs::exists(logFileName)) {
            fs::rename(logFileName, backupLog);
        }
        openLogFile();
    }
};

/*
LogRotator logger("app.log", 1024 * 1024, 5); // 1MB max size, 5 max files
for (int i = 0; i < 1000; ++i) {
    logger.log("This is a test log message " + std::to_string(i));
}

*/
