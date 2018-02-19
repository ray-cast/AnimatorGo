// File: Logger.cpp
// Author: PENGUINLIONG
#include <iostream>
#include "Logger.hpp"
#include "UnitTest.hpp"
#include "../DateTime.hpp"

namespace LiongPlus
{
	namespace Testing
	{
		void Logger::Log(std::string label, std::string msg)
		{
			*UnitTest::Results.Last().Log << DateTime::GetRfc3339(time(nullptr)) << " " << label << " " << msg << std::endl;
		}

		void Logger::Info(std::string msg)
		{
			Log("[INFO]", msg);
		}
		void Logger::Warn(std::string msg)
		{
			Log("[WARN]", msg);
		}
		void Logger::Excp(std::string msg)
		{
			Log("[EXCP]", msg);
		}
		void Logger::Sver(std::string msg)
		{
			Log("[SVER]", msg);
		}
	}
}