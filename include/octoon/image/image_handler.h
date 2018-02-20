#ifndef OCTOON_IMAGE_HANDLER_H_
#define OCTOON_IMAGE_HANDLER_H_

#include <octoon/image/image_types.h>

namespace octoon
{
	namespace image
	{
		class OCTOON_EXPORT ImageHandler
		{
		public:
			ImageHandler() noexcept = default;
			virtual ~ImageHandler() = default;

			virtual bool do_can_read(const char* type_name) const noexcept = 0;
			virtual bool do_can_read(istream& stream) const noexcept = 0;

			virtual bool do_load(istream& stream, Image& image) except = 0;
			virtual bool do_save(ostream& stream, const Image& image) except = 0;

		private:
			ImageHandler(const ImageHandler&) noexcept = delete;
			const ImageHandler& operator=(const ImageHandler&) noexcept = delete;
		};
	}
}

#endif