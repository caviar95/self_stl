#pragma once

#include "intf_log_sink.h"
#include "intf_log_formatter.h"
#include "log_level.h"
#include <memory>

class BusinessLogger {
public:
    BusinessLogger(std::shared_ptr<ILogSink> sink, std::shared_ptr<ILogFormatter> formatter) : sink_(std::move(sink)), formatter_(std::move(formatter)) {
    }

    template <typename... Args>
    void Log(LogLevel level, const std::string& pattern, Args&&... args) {
        std::string msg = formatter_->Format(level, pattern, std::forward<Args>(args)...);
        sink_->Write(level, msg);
    }

private:
    std::shared_ptr<ILogSink> sink_;
    std::shared_ptr<ILogFormatter> formatter_;
};
