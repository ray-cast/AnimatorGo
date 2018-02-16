// File: zstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_ZSTREAM_H_
#define OCTOON_IO_ZSTREAM_H_
#include <istream>
#include <ostream>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include "zipper/zipper.h"
#include "zipper/unzipper.h"
#include "octoon/io/filesystem.h"
#include "octoon/io/mstream.h"

namespace octoon {
namespace io {

class zstream : public std::iostream {
 public:
  zstream(FilesystemPtr fs) noexcept;
  zstream(const zstream&) = delete;
  zstream(zstream&&);

  zstream& operator=(zstream&& rv) noexcept;

  void open(const Orl& orl);
  void close();

 private:
  FilesystemPtr fs_;
  mstream file_;
};

} // namespace io
} // namespace octoon

#endif // OCTOON_IO_ZSTREAM_H_
