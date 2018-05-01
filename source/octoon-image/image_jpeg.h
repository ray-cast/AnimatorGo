#ifndef OCTOON_IMAGE_JPEG_H_
#define OCTOON_IMAGE_JPEG_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class JPEGHandler final : public ImageLoader
		{
		public:
			JPEGHandler() noexcept = default;
			virtual ~JPEGHandler() = default;

			bool doCanRead(istream& stream) const noexcept override;
			bool doCanRead(const char* type_name) const noexcept override;

			bool doLoad(istream& stream, Image& image) noexcept override;
			bool doSave(ostream& stream, const Image& image) noexcept override;

		private:
			JPEGHandler(const JPEGHandler&) noexcept = delete;
			JPEGHandler& operator=(const JPEGHandler&) noexcept = delete;
		};
	}
}

#endif