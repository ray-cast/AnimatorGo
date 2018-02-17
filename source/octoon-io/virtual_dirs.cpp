// File: virtual_dirs.h
// Author: PENGUINLIONG
#include <fstream>
#include <cassert>
#include "octoon/io/virtual_dirs.h"
#include "octoon/io/mstream.h"

namespace octoon {
namespace io {

// LocalDir

namespace detail {

unsigned int
translate_opts(const OpenOptions& opts) {
  unsigned int rv = std::ios_base::binary;
  if (opts.options.read) {
    rv |= std::ios_base::in;
  }
  if (opts.options.write || opts.options.create) {
    rv |= std::ios_base::out;
  }
  if (opts.options.append) {
    rv |= std::ios_base::app;
  }
  if (opts.options.truncate) {
    rv |= std::ios_base::trunc;
  }
  return rv;
}
class LocalFileStream : public stream {
public:
  bool open(const std::string& path, const OpenOptions& opts) {
    inner_.open(path, translate_opts(opts));
    if (inner_.is_open()) {
      opts_ = opts;
      return true;
    } else {
      return false;
    }
  }
  bool can_read() {
    return opts_.options.read;
  }
  bool can_write() {
    return opts_.options.write || opts_.options.create;
  }
  bool can_seek() {
    return true;
  }
  size_t read(uint8_t* buf, size_t size) {
    try {
      inner_.read((char*)buf, size);
    }
    catch (const std::ios_base::failure&) {
      return 0;
    }
    inner_.seekp(inner_.tellg(), std::ios_base::beg);
    return inner_.gcount();
  }
  size_t write(const uint8_t* buf, size_t size) {
    try {
      inner_.write((char*)buf, size);
    } catch (const std::ios_base::failure&) {
      return 0;
    }
    inner_.seekg(inner_.tellp(), std::ios_base::beg);
    return size;
  }
  bool seek(long dist, SeekOrigin ori) {
    try {
      switch (ori) {
      case octoon::io::SeekOrigin::Start:
        inner_.seekg(dist, std::ios_base::beg);
        inner_.seekp(dist, std::ios_base::beg);
        break;
      case octoon::io::SeekOrigin::End:
        inner_.seekg(dist, std::ios_base::end);
        inner_.seekp(dist, std::ios_base::end);
        break;
      case octoon::io::SeekOrigin::Current:
        inner_.seekg(dist, std::ios_base::cur);
        inner_.seekp(dist, std::ios_base::cur);
        break;
      }
      return true;
    }
    catch (const std::ios_base::failure&) {
      return false;
    }
  }
private:
  std::fstream inner_;
  OpenOptions opts_;
};
} // namespace detail

LocalDir::LocalDir(const std::string& base_dir) noexcept :
base_dir_(base_dir) {
}
std::unique_ptr<stream>
LocalDir::open(const Orl& orl, const OpenOptions& options) {
  std::string rv;
  auto path = orl.path();
  if (rv.back() != '/') {
    rv.reserve(base_dir_.size() + 1 + path.size());
    rv.append(base_dir_);
    rv.push_back('/');
    rv.append(path);
  } else {
    rv.reserve(base_dir_.size() + path.size());
    rv.append(base_dir_);
    rv.append(path);
  }
  auto rv_stream = std::make_unique<detail::LocalFileStream>();
  rv_stream->open(path, options);
  return rv_stream;
}

// ZipArchive

ZipArchive::ZipArchive(const std::string& zip_file) :
  unzipper_(zip_file) {
}
std::unique_ptr<stream>
ZipArchive::open(const Orl& orl, const OpenOptions& options) {
  std::vector<uint8_t> buf;
  assert(unzipper_.extractEntryToMemory(orl.path(), buf));
  return std::make_unique<mstream>(buf);
}

} // namespace io
} // namespace octoon
