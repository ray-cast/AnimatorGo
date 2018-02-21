#ifndef OCTOON_IMAGE_TGA_H_
#define OCTOON_IMAGE_TGA_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class TGAHandler final : public ImageHandler
		{
		public:
			TGAHandler() noexcept = default;
			virtual ~TGAHandler() = default;

			bool do_can_read(istream& stream) const noexcept override;
			bool do_can_read(const char* type_name) const noexcept override;

			bool do_load(istream& stream, Image& image) noexcept override;
			bool do_save(ostream& stream, const Image& image) noexcept override;

		private:
			TGAHandler(const TGAHandler&) noexcept = delete;
			TGAHandler& operator=(const TGAHandler&) noexcept = delete;
		};
	}
}

#endif