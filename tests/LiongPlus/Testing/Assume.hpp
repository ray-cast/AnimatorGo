// File: Assume.hpp
// Author: PENGUINLIONG
#pragma once
#include "UnitTest.hpp"

namespace LiongPlus {
namespace Testing {

class Assume
{
public:
	template<typename T>
	static void Equal(T actual, T expectance)
	{
		if (actual != expectance)
		{
			UnitTest::_Results.Last().State = TestState::Skipped;
			UnitTest::_Results.Last().Log.AppendLine(std::string("[Invalid Input(s), skip]"));
		}
	}

	template<typename T, typename ... TArgs>
	static void NoException(T func, TArgs ... args)
	{
		try
		{
			func(args ...);
		}
		catch (...)
		{
			UnitTest::_Results.Last().State = TestState::Skipped;
			UnitTest::_Results.Last().Log.AppendLine(std::string("[Invalid Input(s), skip]"));
		}
	}
};

} // namespace LiongPlus
} // namespace Testing
