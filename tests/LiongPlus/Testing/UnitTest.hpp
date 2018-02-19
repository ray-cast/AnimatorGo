// File: UnitTest.hpp
// Author: PENGUINLIONG
#pragma once

#include <memory>
#include <mutex>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

#ifndef __FUNCTIONW__
#define __FUNCTIONW__ __FUNCTION__
#endif

namespace LiongPlus {
namespace Testing {

enum class TestState
{
	Waiting,
	Passed,
	Failed,
	Skipped
};

class TestObject
{
public:
	TestObject();
	TestObject(const TestObject&) = delete;
	TestObject(TestObject&&) = delete;
	virtual ~TestObject();

	TestObject& operator=(TestObject&&) = delete;

	virtual void Prepare();
	virtual void Test() = 0;
	virtual void CleanUp();

protected:
	void Unit(const std::string& name, std::function<void()> unit);
};

struct TestResult
{
	std::shared_ptr<std::stringstream> Log;
	std::string Name;
	TestState State;

	TestResult();
	TestResult(std::string name);
	TestResult(const TestResult& instance);
	TestResult(TestResult&& instance);

	TestResult& operator=(const TestResult& instance);
	TestResult& operator=(TestResult&& instance);

	bool operator==(TestResult& value);
};

class UnitTest
{
public:
	static class ResultsObject
	{
	public:
		TestResult& operator[](long index)
		{
			return _Results[index];
		}

		void Add(TestResult& result)
		{
			_Results.push_back(result);
		}

		TestResult& Last()
		{
			return _Results.back();
		}
	} Results;

	static void Test(TestObject& obj);

	static void RunUnit(std::function<void(void)> unit);

	static std::string Summary();
	static std::vector<int> ListResultId(TestState state);
private:
	static std::mutex _Mutex;
	static std::vector<TestResult> _Results;
};

} // namespace LiongPlus
} // namespace Testing
