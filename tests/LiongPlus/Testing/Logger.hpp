// File: Logger.hpp
// Author: PENGUINLIONG
#pragma

#include <string>

namespace LiongPlus {
namespace Testing {

class Logger
{
public:
	static void Log(std::string label, std::string msg);

	static void Info(std::string msg);
	static void Warn(std::string msg);
	static void Excp(std::string msg);
	static void Sver(std::string msg);
};

} // namespace LiongPlus
} // namespace Testing
