// File: virtual_dirs.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_VIRTUAL_DIRS_H_
#define OCTOON_IO_VIRTUAL_DIRS_H_
#include "octoon/io/filesystem.h"

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
  bool remove(const Orl& orl, ItemType type = ItemType::File) override;
  ItemType exists(const Orl& orl) override;
private:
  std::string make_path(const Orl& orl) const;

  std::string base_dir_;
};
using LocalDirPtr = std::shared_ptr<LocalDir>;

/*
 * Zip archive as a virtual directory.
 *
 * **NOTE** Zip archives are always read-only. Any non-read options set true
 * will lead to rejection.
 */
class ZipArchive : public VirtualDir {
public:
  ZipArchive(const std::string& zip_file);
  ~ZipArchive();
  std::unique_ptr<stream> open(const Orl& orl,
                               const OpenOptions& options) override;
  bool remove(const Orl& orl, ItemType type = ItemType::File) override;
  ItemType exists(const Orl& orl) override;
private:
  // Hide unzipper.
  void* unzipper_;
  void* entries_;
};
using ZipArchivePtr = std::shared_ptr<ZipArchive>;

} // namespace io
} // namespace octoon

#endif // OCTOON_IO_VIRTUAL_DIRS_H_