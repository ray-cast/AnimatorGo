#include "image_gif.h"

namespace octoon
{
	namespace image
	{
		bool
		GIFHandler::doCanRead(istream& stream) const noexcept
		{
			static const std::uint8_t magic[] = { 'G', 'I', 'F' };

			std::uint8_t hdr[sizeof(magic)];

			if (stream.read((char*)hdr, sizeof(hdr)))
			{
				return std::memcmp(hdr, magic, sizeof(magic)) == 0;
			}

			return false;
		}

		bool
		GIFHandler::doCanRead(const char* type_name) const noexcept
		{
			return std::strncmp(type_name, "gif", 3) == 0;
		}

		bool
		GIFHandler::doLoad(istream&, Image&) noexcept
		{
			return false;
		}

		bool
		GIFHandler::doSave(ostream&, const Image&) noexcept
		{
			return false;
		}
	}
}