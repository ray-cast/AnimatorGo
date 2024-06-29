#ifndef OCTOON_BASE64_H_
#define OCTOON_BASE64_H_

#include <string>
#include <vector>
#include <octoon/runtime/platform.h>

namespace octoon
{
	OCTOON_EXPORT std::string base64_encode(const unsigned char* data, std::size_t len);
	OCTOON_EXPORT std::string base64_encode(const std::string& s);
	OCTOON_EXPORT std::string base64_encode(const std::u8string& s);

	OCTOON_EXPORT std::vector<char> base64_decode(const std::string& s);
}

#endif