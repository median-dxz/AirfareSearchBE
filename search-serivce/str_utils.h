#pragma once

#include <chrono>
#include <string>

using TimePoint = decltype(std::chrono::system_clock::from_time_t(0));

std::string padLeft(const std::string &input, char paddingChar, size_t desiredLength);
TimePoint dateFromString(const std::string &datetimeStr, const std::string &format);
std::string dateToString(const TimePoint &datetime, const std::string &format);