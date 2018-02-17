// File: mstream.cpp
// Author: PENGUINLIONG
#include <cassert>
#include <utility>
#include "octoon/io/mstream.h"

namespace octoon {
namespace io {

mstream::mstream() noexcept :
  buffer_(DEFAULT_BUFFER_SIZE) {
}
mstream::mstream(mstream&& rv) noexcept :
  buffer_(std::move(rv.buffer_)),
  pos_(rv.pos_) {
}
mstream::mstream(size_t capacity) noexcept :
  buffer_(capacity) {
}
mstream::mstream(std::vector<uint8_t> buffer) noexcept :
  buffer_(buffer),
  pos_(0) {
}

mstream&
mstream::operator=(mstream&& rv) noexcept {
  buffer_ = std::move(rv.buffer_);
  pos_ = std::move(rv.pos_);
  return *this;
}

bool mstream::can_read() {
  return true;
}
bool mstream::can_write() {
  return true;
}
bool mstream::can_seek() {
  return true;
}

size_t mstream::read(uint8_t* buf, size_t size) {
  std::lock_guard<std::mutex> guard(lock_);
  auto buf_size = buffer_.size();
  size_t actual_copyable;
  // There is enough bytes to write.
  if (buf_size - pos_ > size) {
    actual_copyable = size;
    pos_ += size;
  // Bytes not enough, write all we can have.
  } else {
    actual_copyable = buf_size - pos_;
    pos_ = buf_size;
  }
  std::memcpy(buf, buffer_.data() + pos_, actual_copyable);
  return actual_copyable;
}
size_t mstream::write(const uint8_t* buf, size_t size) {
  std::lock_guard<std::mutex> guard(lock_);
  // Re-allocation is required to store incoming data.
  buffer_.reserve(pos_ + size);
  std::memcpy(buffer_.data() + pos_, buf, size);
  return size;
}
bool mstream::seek(long dist, SeekOrigin ori) {
  size_t base;
  switch (ori) {
  case octoon::io::SeekOrigin::Start:
    base = 0;
    break;
  case octoon::io::SeekOrigin::End:
    base = pos_;
    break;
  case octoon::io::SeekOrigin::Current:
    base = buffer_.size();
    break;
  }
  size_t resultant = base + dist;
  if (resultant >= 0 && resultant <= buffer_.size()) {
    pos_ = resultant;
    return true;
  }
  return false;
}

std::vector<uint8_t>
mstream::into_buffer() noexcept {
  return std::move(buffer_);
}

} // namespace io
} // namespace octoon
