#include <iostream>
#include <thread>
#include "LiongPlus/Testing/UnitTest.hpp"
#include <chrono>

using namespace LiongPlus::Testing;

void test_octoon_io();

int main() {
  std::cout << "Testing Octoon components..." << std::endl;

  test_octoon_io();

  std::cout << UnitTest::Summary() << std::endl;

  return 0;
}
