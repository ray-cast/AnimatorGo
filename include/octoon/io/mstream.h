// File: mstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_MSTREAM_H_
#define OCTOON_IO_MSTREAM_H_
#include <cstdint>
#include <utility>
#include <iostream>
#include <streambuf>
#include <vector>

namespace octoon {
namespace io {

struct mstream : public std::iostream, std::streambuf {
 public:
  const size_t DEFAULT_BUFFER_SIZE = 4 * 1024;
  const size_t REARRANGE_THRESHOLD = DEFAULT_BUFFER_SIZE / 2;

  using buffer_t = std::vector<uint8_t>;

  mstream() noexcept;
  mstream(const mstream&) = delete;
  mstream(mstream&& rv) noexcept;
  mstream(size_t capacity) noexcept;
  mstream(std::vector<uint8_t> buffer) noexcept;

  mstream& operator=(mstream&& rv) noexcept;

  int underflow() override;
  int overflow(int c) override;

  buffer_t into_buffer() noexcept;

 private:
  buffer_t buffer_;
  size_t pos_;
};

} // namespace io
} // namespace octoon

#endif // OCTOON_IO_MSTREAM_H_
