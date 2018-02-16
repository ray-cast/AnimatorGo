// File: mstream.cpp
// Author: PENGUINLIONG
#include "octoon/io/mstream.h"

namespace octoon {
namespace io {

mstream::mstream() noexcept :
  std::iostream(this),
  buffer_(DEFAULT_BUFFER_SIZE) {
}
mstream::mstream(mstream&& rv) noexcept :
  std::iostream(this),
  buffer_(std::move(rv.buffer_)),
  pos_(rv.pos_) {
}
mstream::mstream(size_t capacity) noexcept :
  std::iostream(this),
  buffer_(capacity) {
}
mstream::mstream(std::vector<uint8_t> buffer) noexcept :
  std::iostream(this),
  buffer_(buffer),
  pos_(0) {
}

mstream&
mstream::operator=(mstream&& rv) noexcept {
  buffer_ = std::move(rv.buffer_);
  pos_ = std::move(rv.pos_);
  return *this;
}

int
mstream::underflow() {
    auto rv = buffer_[pos_++];
    if (pos_ >= REARRANGE_THRESHOLD) {
      buffer_.erase(buffer_.begin(), buffer_.begin() + pos_);
      pos_ = 0;
    }
    return rv;
}
int
mstream::overflow(int c) {
  buffer_.push_back(static_cast<uint8_t>(c));
  return c;
}
std::vector<uint8_t>
mstream::into_buffer() noexcept {
  return std::move(buffer_);
}

} // namespace io
} // namespace octoon
