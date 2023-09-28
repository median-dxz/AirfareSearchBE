#include "str_utils.h"

#include <iomanip>
#include <sstream>

#include <fmt/chrono.h>

using std::string;
using std::chrono::system_clock;

string padLeft(const string &input, char paddingChar, size_t desiredLength) {
    if (input.length() >= desiredLength) {
        return input;
    } else {
        size_t paddingCount = desiredLength - input.length();
        string padding(paddingCount, paddingChar);
        return padding + input;
    }
}

TimePoint dateFromString(const string &datetimeStr, const string &format) {
    std::istringstream datetimeStream(datetimeStr);
    std::tm tm = {};
    datetimeStream >> std::get_time(&tm, format.c_str());
    return system_clock::from_time_t(std::mktime(&tm));
}

string dateToString(const TimePoint &datetime, const string &format) { return fmt::format(format, datetime); }

std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}
