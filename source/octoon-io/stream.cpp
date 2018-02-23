#include <utility>
#include "octoon/io/stream.h"

namespace octoon {
namespace io {
namespace detail {

// StdStreamExt

StdStreamExt::StdStreamExt(stream* src) noexcept :
  src_(src),
  std::iostream(this) {
}
StdStreamExt::StdStreamExt(StdStreamExt&& rv) noexcept :
  src_(rv.src_),
  std::iostream(this) {
}

int
StdStreamExt::overflow(int c) {
  uint8_t byte = (uint8_t)c;
  if (src_->write(&byte, 1)) {
    return c;
  } else {
    return EOF;
  }
}
int
StdStreamExt::underflow() {
  uint8_t byte;
  if (src_->read(&byte, 1)) {
    return byte;
  } else {
    return EOF;
  }
}
}

// Stream

const char*
stream::last_hint() {
  return hint_;
}
detail::StdStreamExt
stream::as_std() {
  return detail::StdStreamExt(this);
}

// OpenOptions

OpenOptions&
OpenOptions::read() {
	options.read = true;
	return *this;
}
OpenOptions&
OpenOptions::write() {
	options.write = true;
	return *this;
}
OpenOptions&
OpenOptions::truncate() {
	options.write = true;
	options.truncate = true;
	return *this;
}
OpenOptions&
OpenOptions::create() {
	options.write = true;
	options.create = true;
	return *this;
}
OpenOptions&
OpenOptions::append() {
	options.write = true;
	options.append = true;
	return *this;
}
} // namespace io
} // namespace octoon