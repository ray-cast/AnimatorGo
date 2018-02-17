// File: virtual_dirs.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_VIRTUAL_DIRS_H_
#define OCTOON_IO_VIRTUAL_DIRS_H_
#include "octoon/io/filesystem.h"
#include "zipper/unzipper.h"

namespace octoon {
namespace io {

/*
* Local directory mapped directly to a virtual directory.
*/
class LocalDir : public VirtualDir {
public:
  LocalDir(const std::string& base_dir) noexcept;
  std::unique_ptr<stream> open(const Orl& orl,
                               const OpenOptions& options) override;
private:
  std::string base_dir_;
};
using LocalDirPtr = std::shared_ptr<LocalDir>;

/*
* Zip archive as a virtual directory.
*/
class ZipArchive : public VirtualDir {
public:
  ZipArchive(const std::string& zip_file);
  std::unique_ptr<stream> open(const Orl& orl,
                               const OpenOptions& options) override;
private:
  zipper::Unzipper unzipper_;
};
using ZipArchivePtr = std::shared_ptr<ZipArchive>;

} // namespace io
} // namespace octoon

#endif // OCTOON_IO_VIRTUAL_DIRS_H_