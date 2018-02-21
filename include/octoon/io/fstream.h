// File: fstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FSTREAM_H_
#define OCTOON_IO_FSTREAM_H_
#include <fstream>
#include <memory>
#include "octoon/io/stream.h"

namespace octoon {
namespace io {
struct Orl;
struct OpenOptions;
class FileSystem;

class OCTOON_EXPORT fstream : public stream {
 public:
  fstream() noexcept;
  fstream(const fstream&) noexcept = delete;
  fstream(fstream&&) noexcept;
  fstream(std::shared_ptr<FileSystem> fs) noexcept;

  fstream& operator=(fstream&&) noexcept;

  /*
   * Try to open the specified resource. If success, `can_read()`, `can_write()`
   * and `can_seek()` SHOULD report according to the capabilities inquired in
   * `options`. Actual reports depends on implementation of virtual directories
   * resources are in. If failed, these reports are always `false`.
   *
   * Any attempt to open a already-opened stream are allowed. After
   * re-opening, all information about the previous opening state is lost.
   *
   * Returns:
   *   `true` on success.
   */
  bool open(const Orl& orl, const OpenOptions& options);
  void close();

  bool can_read() override final;
  bool can_write() override final;
  bool can_seek() override final;

  size_t read(uint8_t* buf, size_t size) override final;
  size_t write(const uint8_t* buf, size_t size) override final;
  bool seek(long dist, SeekOrigin ori = SeekOrigin::Current) override final;

 private:
  std::shared_ptr<FileSystem> fs_;
  std::unique_ptr<stream> inner_;
};
} // namespace io
} // namespace octoon

#endif // OCTOON_IO_FSTREAM_H_
