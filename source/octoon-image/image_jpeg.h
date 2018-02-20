#ifndef OCTOON_IMAGE_JPEG_H_
#define OCTOON_IMAGE_JPEG_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class JPEGHandler final : public ImageHandler
		{
		public:
			JPEGHandler() noexcept = default;
			virtual ~JPEGHandler() = default;

			bool do_can_read(istream& stream) const noexcept;
			bool do_can_read(const char* type_name) const noexcept;

			bool do_load(istream& stream, Image& image) noexcept;
			bool do_save(ostream& stream, const Image& image) noexcept;

		private:
			JPEGHandler(const JPEGHandler&) noexcept = delete;
			JPEGHandler& operator=(const JPEGHandler&) noexcept = delete;
		};
	}
}

#endif