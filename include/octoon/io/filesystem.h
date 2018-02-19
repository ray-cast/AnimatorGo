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
 * File system item types supported by `octoon-io`. This type is used to ensure
 * all FS operations are adopted as expected. `NA` stands for 'not available',
 * meaning that the file was not found or of a type out of capability of
 * `octoon-io`.
 */
enum class ItemType {
  File,
  Directory,
  NA
};

/*
 * A virtual directory in `FileSystem`. Different variants of virtual
 * directories are distinguished by URI scheme.
 */
class VirtualDir {
 public:
  /*
   * Open a file in current virtual directory. For writing, all hierarchy of
   * absent directory will be created. This operation targets files only.
   *
   * Returns:
   *   A pointer to general stream created if succeeded. `nullptr` otherwise.
   *
   */
  virtual std::unique_ptr<stream> open(const Orl& orl,
                                       const OpenOptions& options) = 0;
  /*
   * Remove an item in current virtual directory if and only if it has the
   * corresponding `type`. Type `NA` is always rejected.
   *
   * Returns:
   *   `true` if the file located at `orl` is successfully removed or the file
   *   doesn't exist.
   */
  virtual bool remove(const Orl& orl, ItemType type = ItemType::File) = 0;
  /*
   * Test if a file exists in current virtual directory.
   *
   * Returns:
   *   Type of file found via `orl`. `Unknown` if the item doesn't exist.
   */
  virtual ItemType exists(const Orl& orl) = 0;
};
using VirtualDirPtr = std::shared_ptr<VirtualDir>;

/*
 * ORL -> Octoon Resource Locator.
 *
 * ORLs are composited by two parts: virtual directory entry and path to file in
 * that virtual directory, separated by a colon (`:`). For example:
 * `app-data:images/background12.png` represents file `images/background12.png`
 * in virtual directory `app-data`.
 * 
 * ORL's path part must be a relative one. A absolute path will be rejected when
 * parsing.
 */
struct Orl {
 public:
  Orl() = default;
  Orl(const Orl& lv) = default;
  Orl(Orl&& rv) = default;
  Orl(const std::string& vdir, const std::string& path);

  Orl& operator=(const Orl& lv) = default;

  bool is_valid() const;
  /*
   * Parse a ORL string into a perfectly valid ORL object.
   * 
   * Paths will be sanitized to protect from path traversal attack. So it's
   * recommended to use `parse()` to create ORL objects.
   * 
   * Returns:
   *   True if the string is a valid, parsable ORL; otherwise, false.
   */
  static bool parse(const std::string& orl, Orl& out);

  const std::string& virtual_dir() const;
  const std::string& path() const;
  
  std::string to_string() const;

  Orl Orl::parent() const;
 private:
  std::string vdir_, path_;
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
  /*
   * The following flags depends on `write`. Any call to these helpers will
   * automatically set `write` to true. If `write` is `false`, any `VirtualDir`
   * SHOULD NOT check the following flags.
   */
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
