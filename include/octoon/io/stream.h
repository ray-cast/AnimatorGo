// File: stream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_STREAM_H_
#define OCTOON_IO_STREAM_H_
#include <cstdint>
#include <vector>
#include <iostream>

namespace octoon {
namespace io {

constexpr size_t DEFAULT_BUFFER_SIZE = 8 * 1024;

enum class SeekOrigin {
  Start, End, Current
};

class stream;

namespace detail {
class StdStreamExt final :
  public std::iostream,
  public std::streambuf {
  friend stream;
public:
  StdStreamExt(stream* src) noexcept;
  StdStreamExt(const StdStreamExt&) = delete;
  StdStreamExt(StdStreamExt&&) noexcept;
  int overflow(int c) override final;
  int underflow() override final;

private:
  stream* src_;
};
}

/*
 * Base of all Octoon stream types. Provide fundamental I/O functionalities.
 */
class stream {
public:
  using std_stream = detail::StdStreamExt;

  /*
   * Report the current stream's capability of reading.
   */
  virtual bool can_read() = 0;
  /*
   * Report the current stream's capability of writing.
   */
  virtual bool can_write() = 0;
  /*
   * Report the current stream's capability of seeking.
   */
  virtual bool can_seek() = 0;

  /*
   * Read from source of the current stream and write all written data into
   * provided buffer `buf`. The number of bytes written is AT MOST `size`; but
   * the actual number can be less.
   *
   * Returns:
   *   Number of bytes read.
   */
  virtual size_t read(uint8_t* buf, size_t size) = 0;
  /*
   * Write to source of the current stream using data in provided buffer `buf`
   * The number of bytes written is AT MOST `size`; but the actual number can be
   * less.
   *
   * Returns:
   *   Number of bytes written.
   */
  virtual size_t write(const uint8_t* buf, size_t size) = 0;
  /*
   * Seek in the stream. This operation will move the current position for given
   * distance `dist`. The origin `ori` indicates the position in the stream to
   * start seeking.
   *
   * Returns:
   *   `true` if the operation was finished successfully; `false` otherwise. On
   *   failure, the internal position will not change.
   */
  virtual bool seek(long dist, SeekOrigin ori = SeekOrigin::Current) = 0;

  /*
   * Get the last hint. A hint can be more than an error; any sort or
   * information about the last operation can be reported.
   */
  const char* last_hint();

  /*
   * Allow manipulation of stream in the style recommended by C++ standard, using
   * `operator>>` and `operator<<`.
   */
  std_stream as_std();
protected:
  /*
   * Set the last hint.
   */
  void set_last_hint(const char* hint) {
    hint_ = hint;
  }
private:
  const char* hint_;
};


} // namespace io
} // namespace octoon


#endif // OCTOON_IO_STREAM_H_
