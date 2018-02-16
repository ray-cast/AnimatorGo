// File: fstream.cpp
// Author: PENGUINLIONG
#include "octoon/io/fstream.h"
#include <cassert>

namespace octoon {
namespace io {

fstream::fstream(FilesystemPtr fs) noexcept :
  fs_(fs),
  buf_(),
  std::iostream(&buf_) {
}
fstream::fstream(fstream&& rv) noexcept :
  fs_(std::move(rv.fs_)),
  buf_(std::move(rv.buf_)),
  std::iostream(&buf_) {
}

fstream&
fstream::operator=(fstream&& rv) noexcept {
  fs_ = std::move(rv.fs_);
  buf_ = std::move(rv.buf_);
  return *this;
}

void
fstream::open(const Orl& orl, std::ios_base::openmode mode) {
  auto vdir = fs_->get(orl);
  assert(vdir->is_instance_of("LocalDir"));
  auto local_path = vdir->downcast<LocalDir>()->canonicalize(orl.path());
  buf_.open(local_path, mode);
}
void
fstream::close() {
  buf_.close();
}

} // namespace io
} // namespace octoon
