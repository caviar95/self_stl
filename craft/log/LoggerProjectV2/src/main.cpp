#include "BusinessLogger.h"
#include "PlatformLogger.h"
#include <string>

int main() {
    PlatformLogger platformLogger;
    BusinessLogger logger(platformLogger);

    int number = 42;
    std::string name = "Device";
    int hexVal = 0xABCD;

    logger.Log(0, number, name, hexVal);
    logger.Log(1, "Hello", "world", 123);
    logger.Log(2);  // No parameter

    return 0;
}
