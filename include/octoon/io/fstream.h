// File: fstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FSTREAM_H_
#define OCTOON_IO_FSTREAM_H_
#include <istream>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include "octoon/io/filesystem.h"

namespace octoon {
namespace io {

class fstream : public std::iostream {
 public:
  fstream(FilesystemPtr fs) noexcept;
  fstream(const fstream&) = delete;
  fstream(fstream&&) noexcept;

  fstream& operator=(fstream&& rv) noexcept;

  void open(const Orl& orl,
            std::ios_base::openmode mode = std::ios_base::in);
  void close();

 private:
  FilesystemPtr fs_;
  std::filebuf buf_;
};

} // namespace io
} // namespace octoon

#endif // OCTOON_IO_FSTREAM_H_
