// File: filesystem.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FILESYSTEM_H_
#define OCTOON_IO_FILESYSTEM_H_
#include <map>
#include <memory>
#include <string>

#include "octoon/io/fstream.h"

namespace octoon {
namespace io {

/*
 * A virtual directory in `FileSystem`. Different variants of virtual
 * directories are distinguished by URI scheme.
 */
class VirtualDir {
 public:
  virtual std::unique_ptr<stream> open(const Orl& orl,
                                       const OpenOptions& options) = 0;
};
using VirtualDirPtr = std::shared_ptr<VirtualDir>;

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

  std::string virtual_dir() const;
  std::string path() const;
  
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

  static std::shared_ptr<FileSystem> instance() noexcept;

 private:
  std::map<std::string, std::shared_ptr<VirtualDir>> registry_;
};
using FilesystemPtr = std::shared_ptr<FileSystem>;

/*
 * Options instructing virtual directories how to construct streams.
 */
struct OpenOptions {
public:
  OpenOptions() = default;
  OpenOptions(const OpenOptions&) = default;
  OpenOptions(OpenOptions&&) = default;
  OpenOptions& operator=(const OpenOptions&) = default;

  OpenOptions& read();
  OpenOptions& write();
  OpenOptions& truncate();
  OpenOptions& create();
  OpenOptions& append();

  struct {
    bool read, write, truncate, create, append;
  } options;
};


} // namaspace io
} // namaspace octoon

#endif // OCTOON_IO_FILESYSTEM_H_
