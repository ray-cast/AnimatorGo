// File: zstream.cpp
// Author: PENGUINLIONG
#include "octoon/io/zstream.h"

namespace octoon {
namespace io {

zstream::zstream(FilesystemPtr fs) noexcept :
  fs_(),
  file_(),
  std::iostream(&file_) {
}
zstream::zstream(zstream&& rv) :
  fs_(std::move(rv.fs_)),
  file_(std::move(rv.file_)),
  std::iostream(&file_) {
}

zstream&
zstream::operator=(zstream&& rv) noexcept {
  fs_ = std::move(rv.fs_);
  file_ = std::move(rv.file_);
  return *this;
}

void
zstream::open(const Orl& orl) {
  auto vdir = fs_->get(orl);
  assert(vdir->is_instance_of("ZipArchive"));
  file_ = std::move(vdir->downcast<ZipArchive>()->extract(orl.path()));
}
void
zstream::close() {
  file_.into_buffer();
}

} // namespace io
} // namespace octoon
