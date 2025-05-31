#pragma once

#include <fstream>
#include <string>
#include <vector>

class FileWriter {
public:
    explicit FileWriter(const std::string &path);
    ~FileWriter();

    void Write(const std::vector<std::string>& lines);

private:
    std::ofstream ofs_;
};