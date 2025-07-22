#include "LoggingMacro.hpp"
#include <string>

int main() {
    REPORT(false, 1001, "C", 93);      // Disk C full, usage 93%
    REPORT(false, 1002, "admin", 42);  // User admin not found at line 42
    REPORT(false, 1003, 3.1415926);    // Floating point error
    REPORT(false, 9999);               // Unknown error
    return 0;
}
