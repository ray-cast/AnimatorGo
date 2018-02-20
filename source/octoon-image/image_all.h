#ifndef OCTOON_IMAGE_ALL_H_
#define OCTOON_IMAGE_ALL_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		bool empty_handler() noexcept;
		bool add_handler(ImageHandlerPtr handler) noexcept;
		bool remove_handler(ImageHandlerPtr handler) noexcept;

		ImageHandlerPtr find_handler(const char* type) noexcept;
		ImageHandlerPtr find_handler(istream& stream) noexcept;
		ImageHandlerPtr find_handler(istream& stream, const char* type) noexcept;
	}
}

#endif