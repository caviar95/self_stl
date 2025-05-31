#include "flush_buffer.h"

void FlushBuffer::Add(const std::string &msg) {
    std::lock_guard<std::mutex> lk(mtx_);

    buffer_.emplace_back(msg);
}

std::vector<std::string> FlushBuffer::CollectAndReset() {
    std::lock_guard<std::mutex> lk(mtx_);

    std::vector<std::string> out;

    out.swap(buffer_);

    return out;
}