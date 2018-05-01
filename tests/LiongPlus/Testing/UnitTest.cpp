// File: UnitTest.cpp
// Author: PENGUINLIONG
#include <iostream>
#include <iomanip>
#include <exception>
#include "UnitTest.hpp"

namespace LiongPlus {
namespace Testing {
using std::swap;

std::mutex UnitTest::_Mutex;
std::vector<TestResult> UnitTest::_Results;
UnitTest::ResultsObject UnitTest::Results;

void Logger::Log(std::string label, std::string msg) {
  *UnitTest::Results.Last().Log << DateTime::GetRfc3339(time(nullptr)) << " " << label << " " << msg << std::endl;
}

void Logger::Info(std::string msg) {
  Log("[INFO]", msg);
}
void Logger::Warn(std::string msg) {
  Log("[WARN]", msg);
}
void Logger::Excp(std::string msg) {
  Log("[EXCP]", msg);
}
void Logger::Sver(std::string msg) {
  Log("[SVER]", msg);
}

TestObject::TestObject() {
}
TestObject::~TestObject() {
}

void TestObject::Prepare() {
}
void TestObject::CleanUp() {
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
bool TestObject::Assert(bool condition, const char* description) {
  if (condition) {
    UnitTest::Results.Last().State = TestState::Passed;
    *UnitTest::Results.Last().Log << "[ASSERTION PASSED]" <<
      std::endl;
  }
  else {
    UnitTest::Results.Last().State = TestState::Failed;
    *UnitTest::Results.Last().Log << "[ASSERTION FAILED] " <<
      description << std::endl;
  }
  return !condition;
}
bool TestObject::Assume(bool condition, const char* description) {
  if (condition) {
    UnitTest::Results.Last().State = TestState::Skipped;
    *UnitTest::Results.Last().Log << "[INVALID INPUT] " <<
      description << std::endl;
  }
  return !condition;
}

TestResult::TestResult() :
  Log(std::make_shared<std::stringstream>()),
  Name(std::string()),
  State(TestState::Waiting) {
}
TestResult::TestResult(std::string name) :
  Log(std::make_shared<std::stringstream>()),
  Name(name),
  State(TestState::Waiting) {
}
TestResult::TestResult(const TestResult& instance) :
  Log(instance.Log),
  Name(instance.Name),
  State(instance.State) {
}
TestResult::TestResult(TestResult&& instance) :
  TestResult() {
  swap(Log, instance.Log);
  swap(Name,  instance.Name);
  swap(State, instance.State);
}

TestResult& TestResult::operator=(const TestResult& instance) {
  Log = instance.Log;
  Name = instance.Name;
  State = instance.State;
  return *this;
}
TestResult& TestResult::operator=(TestResult&& instance) {
  swap(Log, instance.Log);
  swap(Name, instance.Name);
  swap(State, instance.State);
  return *this;
}

bool TestResult::operator==(TestResult& value) {
  return Name == value.Name;
}

void UnitTest::Test(TestObject& obj) {
  obj.Prepare();
  obj.Test();
  obj.CleanUp();
}

void UnitTest::Test(TestObject&& obj) {
	obj.Prepare();
	obj.Test();
	obj.CleanUp();
}

void UnitTest::RunUnit(std::function<void(void)> unit) {
  std::lock_guard<std::mutex> guard(_Mutex);
  try {
    unit();
  } catch (const std::exception& e) {
    *_Results.back().Log << "[Exception occured] " << e.what() << std::endl;
    _Results.back().State = TestState::Failed;
    return;
  } catch (...) {
    *_Results.back().Log << "[Exception occured]" << std::endl;
    _Results.back().State = TestState::Failed;
    return;
  }
  if (_Results.back().State == TestState::Waiting) {
    _Results.back().State = TestState::Passed;
  }
}

std::string UnitTest::Summary() {
  std::lock_guard<std::mutex> guard(_Mutex);
  long passed = 0;
  for (auto& result : _Results) {
    if (result.State == TestState::Passed) {
      ++passed;
    }
  }
  return std::to_string(_Results.size()) + " tests have been run and " + std::to_string(passed) + " of them passed.";
}
std::vector<int> UnitTest::ListResultId(TestState state) {
  std::lock_guard<std::mutex> guard(_Mutex);
  std::vector<int> list;
  for (int i = 0; i < _Results.size(); ++i) {
    if (_Results[i].State == state) {
      list.push_back(i);
    }
  }
  return list;
}
} // namespace Testing
} // namespace LiongPlus