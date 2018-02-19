// File: UnitTest.cpp
// Author: PENGUINLIONG
#include <iostream>
#include <iomanip>
#include "UnitTest.hpp"

namespace LiongPlus
{
	namespace Testing
	{
		using std::swap;

		std::mutex UnitTest::_Mutex;
		std::vector<TestResult> UnitTest::_Results;
		UnitTest::ResultsObject UnitTest::Results;

		TestObject::TestObject()
		{
		}
		TestObject::~TestObject()
		{
		}

		void TestObject::Prepare()
		{
		}
		void TestObject::CleanUp()
		{
		}

		const char* GetTestStateLiteral(TestState state) {
			switch (state) {
			case TestState::Passed:
				return "Passed";
			case TestState::Failed:
				return "Failed";
			case TestState::Skipped:
				return "Skipped";
			default:
				return "?";
			}
		}
		void TestObject::Unit(const std::string& name, std::function<void()> unit) {
			std::cout << name << " ";
			UnitTest::Results.Add(TestResult(name));
			UnitTest::RunUnit(unit);
			auto state = UnitTest::Results.Last().State;
			std::cout << std::setfill('.') << std::setw(70 - name.size()) <<
				" " << GetTestStateLiteral(state) << std::endl;
			if (state != TestState::Passed) {
				std::cout << "# " << name << " log begin --" << std::endl;
				std::cout << UnitTest::Results.Last().Log->str();
				std::cout << "# " << name << " log end   --" << std::endl;
			}
		}



		TestResult::TestResult()
			: Log(std::make_shared<std::stringstream>())
			, Name(std::string())
			, State(TestState::Waiting)
		{
		}
		TestResult::TestResult(std::string name)
			: Log(std::make_shared<std::stringstream>())
			, Name(name)
			, State(TestState::Waiting)
		{
		}
		TestResult::TestResult(const TestResult& instance)
			: Log(instance.Log)
			, Name(instance.Name)
			, State(instance.State)
		{
		}
		TestResult::TestResult(TestResult&& instance)
			: TestResult()
		{
			swap(Log, instance.Log);
			swap(Name,  instance.Name);
			swap(State, instance.State);
		}

		TestResult& TestResult::operator=(const TestResult& instance)
		{
			Log = instance.Log;
			Name = instance.Name;
			State = instance.State;
			return *this;
		}
		TestResult& TestResult::operator=(TestResult&& instance)
		{
			swap(Log, instance.Log);
			swap(Name, instance.Name);
			swap(State, instance.State);
			return *this;
		}

		bool TestResult::operator==(TestResult& value)
		{
			return Name == value.Name;
		}



		void UnitTest::Test(TestObject& obj)
		{
			obj.Prepare();
			obj.Test();
			obj.CleanUp();
		}

		void UnitTest::RunUnit(std::function<void(void)> unit)
		{
			_Mutex.lock();
			try
			{
				unit();
			}
			catch (...)
			{
				*_Results.back().Log << "[Exception occured]" << std::endl;
				_Results.back().State = TestState::Failed;
				_Mutex.unlock();
				return;
			}
			if (_Results.back().State == TestState::Waiting)
				_Results.back().State = TestState::Passed;
			_Mutex.unlock();
		}

		std::string UnitTest::Summary()
		{
			long passed = 0;
			for (auto& result : _Results)
			{
				if (result.State == TestState::Passed)
					++passed;
			}
			return std::to_string(_Results.size()) + " tests have been run and " + std::to_string(passed) + " of them passed.";
		}
		std::vector<int> UnitTest::ListResultId(TestState state)
		{
			_Mutex.lock();
			std::vector<int> list;
			for (int i = 0; i < _Results.size(); ++i)
			{
				if (_Results[i].State == state)
					list.push_back(i);
			}
			_Mutex.unlock();
			return list;
		}
	}
}