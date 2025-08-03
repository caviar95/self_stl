#include "business_logger.h"
#include "console_sink.h"
#include "simple_formatter.h"

int main() {
    auto sink = std::make_shared<ConsoleSink>();
    auto formatter = std::make_shared<SimpleFormatter>();
    BusinessLogger logger(sink, formatter);

    logger.Log(LogLevel::Info, "Connecting to {}", "192.168.1.1");
    logger.Log(LogLevel::Error, "Error code: {}", 404);
    logger.Log(LogLevel::Debug, "Value x: {}, y: {}", 10, 20);

    return 0;
}