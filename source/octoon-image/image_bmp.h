#ifndef OCTOON_IMAGE_BMP_H_
#define OCTOON_IMAGE_BMP_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		struct BITMAPINFO;

		class BMPHandler final : public ImageHandler
		{
		public:
			BMPHandler() noexcept = default;
			virtual ~BMPHandler() = default;

			bool do_can_read(istream& stream) const noexcept;
			bool do_can_read(const char* type_name) const noexcept;

			bool do_load(istream& stream, Image& image) except;
			bool do_save(ostream& stream, const Image& image) except;

		private:
			bool decode(istream& stream, Image& image, const BITMAPINFO& info);
			bool encode(istream& stream, Image& image, const BITMAPINFO& info);
			bool loadDIB(istream& stream, Image& image, const BITMAPINFO& info);
		};
	}
}

#endif