// File: filesystem.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FILESYSTEM_H_
#define OCTOON_IO_FILESYSTEM_H_
#include <map>
#include <memory>
#include <string>
#include "zipper/unzipper.h"

#include "octoon/runtime/rtti.h"
#include "octoon/runtime/rtti_interface.h"
#include "octoon/io/mstream.h"

namespace octoon {
namespace io {
/*
 * A virtual directory in `FileSystem`. Different variants of virtual
 * directories are distinguished by URI scheme.
 */
class VirtualDir : public runtime::RttiInterface {
  OctoonDeclareSubInterface(VirtualDir, runtime::RttiInterface);
};
using VirtualDirPtr = std::shared_ptr<VirtualDir>;

/*
 * Local directory mapped directly to a virtual directory.
 */
class LocalDir: public VirtualDir {
  OctoonDeclareSubClass(LocalDir, VirtualDir);
 public:
  LocalDir(const std::string& base_dir) noexcept;
  const std::string& canonicalize(const std::string& rel_path) const noexcept;
 private:
  std::string base_dir_;
};
using LocalDirPtr = std::shared_ptr<LocalDir>;

/*
 * Zip archive as a virtual directory.
 */
class ZipArchive : public VirtualDir {
  OctoonDeclareSubClass(ZipArchive, VirtualDir);
 public:
  ZipArchive(const std::string& zip_file);
  std::vector<uint8_t> extract(const std::string& file) noexcept;
 private:
  zipper::Unzipper unzipper_;
};
using ZipArchivePtr = std::shared_ptr<ZipArchive>;

/*
 * ORL -> Octoon Resource Locator.
 *
 * ORLs are composited by two parts: virtual directory entry and path to file in
 * that virtual directory, separated by a colon (`:`). For example:
 * `app-data:images/background12.png` represents file `images/background12.png`
 * in virtual directory `app-data`.
 */
struct Orl {
 public:
  Orl() = default;
  Orl(const Orl& lv) = default;
  Orl(Orl&& rv) = default;
  Orl(const std::string& orl);
  Orl(const std::string& vdir, const std::string& path);

  bool is_valid() const;

  const std::string& virtual_dir() const;
  const std::string& path() const;
  
  const std::string& as_string() const;
 private:
  std::string orl_;
  size_t colon_pos_;
};

/*
 * A `FileSystem` is a namespace for Octoon instance to seek for resources in
 * local storage, at remote host, or in compressed archive. 
 */
class FileSystem {
 public:
  /*
   * Register an entry to the filesystem, so that the file contained in that
   * entry an be found.
   */
  void reg(const std::string& vpath, VirtualDirPtr entry);
  VirtualDirPtr unreg(const std::string& path);
  VirtualDirPtr get(const Orl& orl) const;

 private:
  std::map<std::string, std::shared_ptr<VirtualDir>> registry_;
};
using FilesystemPtr = std::shared_ptr<FileSystem>;

} // namaspace io
} // namaspace octoon

#endif // OCTOON_IO_FILESYSTEM_H_
