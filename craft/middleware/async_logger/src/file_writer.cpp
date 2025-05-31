#include "file_writer.h"

FileWriter::FileWriter(const std::string &path) {
    ofs_.open(path, std::ios::out | std::ios::app);
}

FileWriter::~FileWriter() {
    ofs_.close();
}

void FileWriter::Write(const std::vector<std::string>& lines) {
    for (const auto& line : lines) {
        ofs_ << line << "\n";
    }

    ofs_.flush();
}
