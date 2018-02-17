// File: fstream.cpp
// Author: PENGUINLIONG
#include <cassert>
#include "octoon/io/fstream.h"
#include "octoon/io/filesystem.h"

namespace octoon {
namespace io {

// OpenOptions

OpenOptions&
OpenOptions::read() {
  options.read = true;
  return *this;
}
OpenOptions&
OpenOptions::write() {
  options.write = true;
  return *this;
}
OpenOptions&
OpenOptions::truncate() {
  options.truncate = true;
  return *this;
}
OpenOptions&
OpenOptions::create() {
  options.create = true;
  return *this;
}
OpenOptions&
OpenOptions::append() {
  options.append = true;
  return *this;
}

// fstream

fstream::fstream() noexcept :
  fs_(FileSystem::instance()) {
}
fstream::fstream(std::shared_ptr<FileSystem> fs) noexcept :
  fs_(fs) {
}
fstream::fstream(fstream&& rv) noexcept :
  fs_(std::move(rv.fs_)) {
}

fstream&
fstream::operator=(fstream&& rv) noexcept {
  fs_ = std::move(rv.fs_);
  return *this;
}

bool
fstream::open(const Orl& orl, const OpenOptions& options) {
  auto vdir = fs_->get(orl);
  inner_ = vdir->open(orl.path(), options);
  return inner_ == nullptr;
}
void
fstream::close() {
  inner_ = nullptr;
}


bool
fstream::can_read() {
  return true;
}
bool
fstream::can_write() {
  return true;
}
bool
fstream::can_seek() {
  return true;
}

size_t
fstream::read(uint8_t* buf, size_t size) {
  return inner_->read(buf, size);
}
size_t
fstream::write(const uint8_t* buf, size_t size) {
  return inner_->write(buf, size);
}
bool
fstream::seek(long dist, SeekOrigin ori) {
  return inner_->seek(dist, ori);
}

} // namespace io
} // namespace octoon
