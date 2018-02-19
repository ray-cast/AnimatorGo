// File: filesystem.cpp
// Author: PENGUINLIONG
#include <cassert>
#include <algorithm>
#include "octoon/io/filesystem.h"

namespace octoon {
namespace io {

// Orl

bool Orl::parse(const std::string& orl, Orl& out) {
  size_t vdir_size = orl.find(':');
  if (vdir_size == orl.size()) {
    return false;
  }
  size_t path_size = orl.size() - vdir_size - 1;
  if (vdir_size == 0 || path_size == 0) {
    return false;
  }
  out.orl_ = orl;
  out.colon_pos_ = vdir_size;
  return true;
}
Orl::Orl(const std::string& vdir, const std::string& path) :
  orl_(),
  colon_pos_() {
  if (vdir.size() == 0 || path.size() == 0) {
    return;
  }
  orl_.reserve(vdir.size() + 1 + path.size());
  orl_.append(vdir);
  orl_.push_back(':');
  orl_.append(path);
  colon_pos_ = vdir.size();
}
bool
Orl::is_valid() const {
  return colon_pos_ > 0 && // `virtual_dir` not empty.
    orl_.size() > colon_pos_ + 1; // `path` not empty.
}
std::string
Orl::virtual_dir() const {
  return orl_.substr(0, colon_pos_);
}
std::string
Orl::path() const {
  return orl_.substr(colon_pos_ + 1);
}
const std::string&
Orl::as_string() const {
  return orl_;
}

// FileSystem
void
FileSystem::reg(const std::string& vpath, VirtualDirPtr vdir) {
  registry_.insert(std::make_pair(vpath, vdir));
}
VirtualDirPtr
FileSystem::unreg(const std::string& vdir) {
  auto it = registry_.find(vdir);
  auto ptr = std::move(it->second);
  registry_.erase(it);
  return ptr;
}
std::string
sanitize_path(const std::string& path) {
  std::string rv;
  auto pos = path.begin();
  auto end = path.end();
  if (*pos != '/') {
    rv += "/";
  } else {
    ++pos;
  }
  while (pos != end) {
    auto next_slash = std::find(pos, end, '/');
    switch (std::distance(pos, next_slash)) {
     case 1:
      if (*pos == '.') {
        continue;
      }
      break;
     case 2:
      if (*pos == '.' && *(pos + 1) == '.') {
          rv.resize(rv.find_last_of("/", rv.size()));
        continue;
      }
      break;
    }
    rv.push_back('/');
    rv.append(pos, next_slash);
    pos = next_slash;
    ++pos;
  }
  return rv;
}
VirtualDirPtr
FileSystem::get(const Orl& orl) const {
  auto it = registry_.find(orl.virtual_dir());
  if (it != registry_.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

std::shared_ptr<FileSystem> FileSystem::instance() noexcept {
  static std::shared_ptr<FileSystem> fs;
  if (fs == nullptr) {
    fs = std::make_shared<FileSystem>();
  }
  return fs;
}

} // namespace io
} // namespace octoon
