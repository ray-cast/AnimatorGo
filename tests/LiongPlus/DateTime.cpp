// File: DateTime.cpp
// Author: PENGUINLIONG
#include <chrono>
#include "DateTime.hpp"

namespace LiongPlus {

namespace DateTime
{
	using namespace std;

	time_t Now()
	{
		using namespace std::chrono;
		return system_clock::to_time_t(system_clock::now());
	}

	enum
	{
		__MinBufferSize = 32,
	};

	string GetDateTime(const string& format, const tm* timeFactors)
	{
		static auto buffer = std::string(__MinBufferSize, 0);
		size_t len = 0;
		while (true) {
			len = strftime((char*)(buffer.data()), buffer.capacity(), format.c_str(), timeFactors);
			if (len == 0)
				buffer.reserve(buffer.capacity() * 2);
			else break;
		}
		buffer.resize(len);
		return buffer;
	}

	string GetRfc1123(time_t t)
	{
		return GetDateTime("%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
	}
	string GetRfc3339(time_t t)
	{
		return GetDateTime("%FT%TZ", gmtime(&t));
	}

	string GetRfc850(time_t t)
	{
		return GetDateTime("%A, %d-%b-%y %H:%M:%S GMT", gmtime(&t));
	}

	string GetAsctimeGmt(time_t t)
	{
		return asctime(gmtime(&t));
	}

	string GetCustomized(const string& format, time_t t)
	{
		return GetDateTime(format, gmtime(&t));
	}
}

} // namespace LiongPlus
