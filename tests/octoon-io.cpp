// File: octoon-io.cpp
// Author: PENGUINLIONG
#include <vector>
#include <string>
#include "octoon/io/filesystem.h"
#include "octoon/io/fstream.h"
#include "octoon/io/virtual_dirs.h"
#include "LiongPlus/Testing/UnitTest.hpp"

using namespace LiongPlus::Testing;
using namespace octoon::io;

class OctoonIoTestObject : public TestObject {
  void Prepare() override {
    // Prepare virtual directories.
    auto inst = FileSystem::instance();
    inst->reg("dir", std::make_shared<LocalDir>("./testenv/io/octoon-file"));
    inst->reg("zip", std::make_shared<ZipArchive>("./testenv/io/octoon-zip.zip"));
  }

  static Orl gen_read_orl(size_t index) {
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
  static Orl gen_write_orl(size_t index) {
    static std::vector<std::string> orls = {
      "dir:write.txt",
      "dir:created-dir/write.txt"
      // `ZipArchive` doesn't allow writing.
    };
    Orl out;
    Orl::parse(orls[index], out);
    return out;
  }

  static void test_orl_parse(const std::string& orl, const std::string& vdir, const std::string& path) {
    Orl out;
    ASSERT(Orl::parse(orl, out));
    ASSERT(out.is_valid());
    ASSERT(vdir == out.virtual_dir());
    ASSERT(path == out.path());
  }
  static void fail_orl_parse(const std::string& orl) {
    Orl out;
    ASSERT(!Orl::parse(orl, out));
    ASSERT(out.virtual_dir().size() == 0);
    ASSERT(out.path().size() == 0);
  }
  static void test_fstream_read(size_t orl_idx) {
    Logger::Info("Read test starts. Constructing `fstream`...");
    fstream file;
    
    Logger::Info("Opening file...");
    ASSERT(file.open(gen_read_orl(orl_idx), OpenOptions().read()));
    std::string buf(4, 0);

    Logger::Info("Reading from file...");
    ASSERT(file.read((uint8_t*)buf.data(), buf.capacity()) == 4);
    
    Logger::Info("Asserts.");
    ASSERT(buf == "Test");
  }
  static void test_fstream_write(size_t orl_idx) {
    Logger::Info("Write test starts. Constructing `fstream`...");
    fstream file;

    Logger::Info("Opening file in write mode...");
    auto file_name = gen_write_orl(orl_idx);
    ASSERT(file.open(file_name, OpenOptions().create()));
    std::string write_buf = "Test";
    
    Logger::Info("Writing to file...");
    ASSERT(file.write((const uint8_t*)write_buf.data(), write_buf.size()) == 4);

    Logger::Info("Opening file...");
    ASSERT(file.open(file_name, OpenOptions().read()));
    std::string read_buf(4, 0);
    
    Logger::Info("Reading back from file...");
    ASSERT(file.read((uint8_t*)read_buf.data(), read_buf.capacity()));
    
    Logger::Info("Asserts about reading.");
    ASSERT(read_buf == "Test");
    file.close();
  }

  static Orl gen_dir_orl(size_t index) {
    static std::vector<std::string> orls = {
      "dir:octoon-file",
      "zip:octoon-file"
    };
    Orl out;
    Orl::parse(orls[index], out);
    return out;
  }
  static void test_file_exists(size_t orl_idx) {
    auto orl = gen_read_orl(orl_idx);
    auto inst = FileSystem::instance();
    ASSERT(inst->get(orl)->exists(orl) == ItemType::File);
  }
  static void test_dir_exists(size_t orl_idx) {
    auto orl = gen_dir_orl(orl_idx);
    auto inst = FileSystem::instance();
    ASSERT(inst->get(orl)->exists(orl) == ItemType::Directory);
  }
  
  void Test() override {

    // `Orl`.

    Unit("test_orl_parsing", []{
      test_orl_parse("vdir:file", "vdir", "file");
      test_orl_parse("vdir:file.ext", "vdir", "file.ext");
      test_orl_parse("vdir:path/file.ext", "vdir", "path/file.ext");
    });
    Unit("test_orl_parent", [] {
      Orl orl;
      ASSERT(Orl::parse("vdir:file.ext", orl));
      ASSERT(orl.parent().to_string() == "vdir:");
      ASSERT(Orl::parse("vdir:path/file.ext", orl));
      ASSERT(orl.parent().to_string() == "vdir:path");
    });

    Unit("fail_orl_parsing", []{
      fail_orl_parse("");
      fail_orl_parse("orl");
      fail_orl_parse(".");
      fail_orl_parse("..");
      fail_orl_parse("orl:");
      fail_orl_parse(":");
      fail_orl_parse(":abc");
      fail_orl_parse(":/");
      fail_orl_parse("://");
      fail_orl_parse("/");
      fail_orl_parse("//");
    });

    // Item existance.

    Unit("test_exists_local_dir_file",          []{ test_file_exists(0); });
    Unit("test_exists_local_dir_file_in_dir",   []{ test_file_exists(1); });
    Unit("test_exists_zip_archive_file",        []{ test_file_exists(2); });
    Unit("test_exists_zip_archive_file_in_dir", []{ test_file_exists(3); });

    Unit("test_exists_local_dir_dir",   []{ test_file_exists(0); });
    Unit("test_exists_zip_archive_dir", []{ test_file_exists(1); });

    // `fstream` read.

    Unit("test_fstream_read_local_dir_file",          []{ test_fstream_read(0); });
    Unit("test_fstream_read_local_dir_file_in_dir",   []{ test_fstream_read(1); });
    Unit("test_fstream_read_zip_archive_file",        []{ test_fstream_read(2); });
    Unit("test_fstream_read_zip_archive_file_in_dir", []{ test_fstream_read(3); });

    // `fstream` write.

    Unit("fail_fstream_write_check_no_creation", [] {
      fstream file;
      auto file_name = gen_write_orl(0);
      ASSERT(!file.open(file_name, OpenOptions().write()));
    });

    Unit("test_fstream_write_local_dir_file",        []{ test_fstream_write(0); });
    Unit("test_fstream_write_local_dir_file_in_dir", []{ test_fstream_write(1); });

    // Item removal.

    Unit("fail_remove_file_wrong_type", []{
      auto orl = gen_write_orl(0);
      ASSERT(!FileSystem::instance()->get(orl)->remove(orl, ItemType::Directory));
      ASSERT(FileSystem::instance()->get(orl)->exists(orl) == ItemType::File);
    });
    Unit("test_remove_file", []{
      auto orl = gen_write_orl(0);
      ASSERT(FileSystem::instance()->get(orl)->remove(orl, ItemType::File));
      ASSERT(FileSystem::instance()->get(orl)->exists(orl) == ItemType::NA);
    });
    Unit("test_remove_dir_with_file", [] {
      auto orl = gen_write_orl(1).parent();
      ASSERT(FileSystem::instance()->get(orl)->remove(orl, ItemType::Directory));
      ASSERT(FileSystem::instance()->get(orl)->exists(orl) == ItemType::NA);
    });
  }
};

void test_octoon_io() {
  UnitTest::Test(OctoonIoTestObject());
}
