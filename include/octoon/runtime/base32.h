#ifndef OCTOON_BASE32_H_
#define OCTOON_BASE32_H_

#include <string>
#include <octoon/runtime/platform.h>

namespace octoon
{
	OCTOON_EXPORT std::string base32_encode(const unsigned char* data, std::size_t len);
	OCTOON_EXPORT std::string base32_encode(const std::string& s);
	OCTOON_EXPORT std::string base32_encode(const std::u8string& s);

	OCTOON_EXPORT std::string base32_decode(const std::string& s);
}

#endif