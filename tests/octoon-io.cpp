// File: octoon-io.cpp
// Author: PENGUINLIONG
#include <vector>
#include <string>
#include "octoon/io/filesystem.h"
#include "octoon/io/fstream.h"
#include "octoon/io/virtual_dirs.h"
#include "LiongPlus/Testing/UnitTest.hpp"
#include "LiongPlus/Testing/Assert.hpp"
#include "LiongPlus/Testing/Logger.hpp"

using namespace LiongPlus::Testing;
using namespace octoon::io;

class OctoonIoTestObject : public TestObject {
  void Prepare() override {
    // Prepare virtual directories.
    auto inst = FileSystem::instance();
    inst->reg("dir", std::make_shared<LocalDir>("./testenv/io/octoon-file"));
    inst->reg("zip", std::make_shared<ZipArchive>("./testenv/io/octoon-zip.zip"));
  }

  static Orl gen_orl(size_t index) {
    static std::vector<std::string> orls = {
      "dir:read.txt",
      "dir:octoon-file/read.txt",
      "zip:read.txt",
      "zip:octoon-file/read.txt"
    };
    Orl out;
    Orl::parse(orls[index], out);
    return out;
  }

  static void test_fstream_read(size_t orl_idx) {
    Logger::Info("Read test starts. Constructing `fstream`...");
    fstream file;
    Logger::Info("Opening file...");
    file.open(gen_orl(orl_idx), OpenOptions().read());
    std::string buf(4, 0);
    Logger::Info("Reading from file...");
    file.read((uint8_t*)buf.data(), buf.capacity());
    Logger::Info("Asserts.");
    Assert::Equal(buf, "Test");
    Assert::Equal(std::string(), file.last_hint());
  }

  void Test() override {
    Unit("test_fstream_read_local_dir_file", []{
      test_fstream_read(0);
    });
    Unit("test_fstream_read_local_dir_file_in_dir", []{
      test_fstream_read(1);
    });
    Unit("test_fstream_read_zip_archive_file", []{
      test_fstream_read(2);
    });
    Unit("test_fstream_read_zip_archive_file_in_dir", []{
      test_fstream_read(3);
    });
  }
};

void test_octoon_io() {
  UnitTest::Test(OctoonIoTestObject());
}
