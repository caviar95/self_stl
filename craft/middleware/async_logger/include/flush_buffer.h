#pragma once

#include <vector>
#include <string>
#include <mutex>

class FlushBuffer {
public:
    void Add(const std::string &msg);
    std::vector<std::string> CollectAndReset();

private:
    std::mutex mtx_;
    std::vector<std::string> buffer_;
};