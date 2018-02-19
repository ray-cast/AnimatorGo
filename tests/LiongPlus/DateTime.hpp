// File: DateTime.hpp
// Author: PENGUINLIONG
#pragma once
#include <ctime>
#include <string>

namespace LiongPlus {

namespace DateTime
{
	time_t Now();

	std::string GetRfc1123(time_t t);
	std::string GetRfc3339(time_t t);
	std::string GetRfc850(time_t t);
	std::string GetAsctimeGmt(time_t t);
	std::string GetCustomized(const std::string& format, time_t t);
}

} // namespace LiongPlus
