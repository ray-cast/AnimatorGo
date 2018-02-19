// File: fstream.cpp
// Author: PENGUINLIONG
#include <cassert>
#include "octoon/io/fstream.h"
#include "octoon/io/filesystem.h"

namespace octoon {
namespace io {

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
  inner_ = nullptr; // Re-open should stash the previous one.
  auto vdir = fs_->get(orl);
  if (vdir == nullptr) {
    return false;
  }
  inner_ = vdir->open(orl, options);
  return inner_ != nullptr;
}
void
fstream::close() {
  inner_ = nullptr;
}


bool
fstream::can_read() {
  return inner_ != nullptr && inner_->can_read();
}
bool
fstream::can_write() {
  return inner_ != nullptr && inner_->can_write();
}
bool
fstream::can_seek() {
  return inner_ != nullptr && inner_->can_seek();
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
