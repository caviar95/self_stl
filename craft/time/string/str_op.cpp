#include <iostream>
#include <string>
#include <regex>
#include <iomanip>
#include <ctime>
#include <chrono>

bool IsValidTimeStamp(const std::string& timestamp) {
    std::regex pattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{1,3}$)");
    return std::regex_match(timestamp, pattern);
}

bool ParseTimestamp(const std::string& timestamp, std::tm& outTm, int& milliseconds) {
    std::istringstream ss(timestamp);
    ss >> std::get_time(&outTm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return false;
    }

    char dot;
    ss >> dot >> milliseconds;
    if (dot != '.' || ss.fail()) {
        return false;
    }

    return true;
}

std::string SubtractTime(const std::string& timestamp, bool subtractHour = true) {
    if (!IsValidTimeStamp(timestamp)) {
        return "Invalid timestamp format";
    }

    std::tm tm{};
    int milliseconds{};

    if (!ParseTimestamp(timestamp, tm, milliseconds)) {
        return "Parsing error";
    }

    std::time_t time = std::mktime(&tm);
    if (time == -1) {
        return "Time conversion error";
    }

    using namespace std::chrono;

    system_clock::time_point tp = system_clock::from_time_t(time);
    tp -= subtractHour ? hours(1) : hours(24);

    std::time_t newTime = system_clock::to_time_t(tp);
    std::tm* newTm = std::localtime(&newTime);

    std::ostringstream oss;
    oss << std::put_time(newTm, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << milliseconds;
    return oss.str();
}

int main() {
    std::string ts = "2025-07-29 14:45:12.123";

    std::cout << "origin time: " << ts << std::endl;
    std::cout << "substrcut one hour: " << SubtractTime(ts, true) << std::endl;
    std::cout << "subtract one day: " << SubtractTime(ts, false) << std::endl;

    return 0;
}
