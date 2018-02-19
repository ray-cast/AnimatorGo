// File: Assert.hpp
// Author: PENGUINLIONG
#pragma once

namespace LiongPlus {
namespace Testing {

class Assert
{
public:
	template<typename TA, typename TB>
	static void Equal(const TA& actual, const TB& expect) {
		if (actual == expect)
		{
			UnitTest::Results.Last().State = TestState::Passed;
			*UnitTest::Results.Last().Log << "[Assertion passed]" << std::endl;
		}
		else
		{
			UnitTest::Results.Last().State = TestState::Failed;
			*UnitTest::Results.Last().Log << "[Assertion failed] Expected: `" <<
				expect << "`; actual: `" << actual << "`." << std::endl;
		}
	}

	template<typename TA, typename TB>
	static void NotEqual(const TA& actual, const TB& expect) {
		if (actual != expect)
		{
			UnitTest::Results.Last().State = TestState::Passed;
			*UnitTest::Results.Last().Log << "[Assertion passed]" << std::endl;
		}
		else
		{
			UnitTest::Results.Last().State = TestState::Failed;
			*UnitTest::Results.Last().Log << "[Assertion failed] Expected: `" <<
				expect << "`; actual: `" << actual << "`." << std::endl;
		}
	}
};

} // namespace LiongPlus
} // namespace Testing
