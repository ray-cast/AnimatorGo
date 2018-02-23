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
#include "../DateTime.hpp"

#ifndef __FUNCTIONW__
#define __FUNCTIONW__ __FUNCTION__
#endif

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

	// Return `true` if assertion failed.
	static bool Assert(bool condition, const char* description = "");

	// Return `true` if assumption failed.
	static bool Assume(bool condition, const char* description = "");
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

		void Add(TestResult&& result)
		{
			_Results.push_back(std::move(result));
		}

		TestResult& Last()
		{
			return _Results.back();
		}
	} Results;

	static void Test(TestObject& obj);
	static void Test(TestObject&& obj);

	static void RunUnit(std::function<void(void)> unit);

	static std::string Summary();
	static std::vector<int> ListResultId(TestState state);
private:
	static std::mutex _Mutex;
	static std::vector<TestResult> _Results;
};
} // namespace Testing
} // namespace LiongPlus

#define ASSERT(condition) if (Assert(condition)) { return; }
#define ASSERT_RICH(condition, description) if (Assert(condition, description)) { return; }
#define ASSUME(condition) if (Assume(condition)) { return; }
#define ASSUME_RICH(condition, description) if (Assume(condition, description)) { return; }