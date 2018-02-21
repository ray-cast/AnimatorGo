#ifndef OCTOON_IMAGE_PNG_H_
#define OCTOON_IMAGE_PNG_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class PNGHandler final : public ImageHandler
		{
		public:
			PNGHandler() noexcept = default;
			virtual ~PNGHandler() = default;

			bool do_can_read(istream& stream) const noexcept override;
			bool do_can_read(const char* type_name) const noexcept override;

			bool do_load(istream& stream, Image& image) noexcept override;
			bool do_save(ostream& stream, const Image& image) noexcept override;

		private:
			PNGHandler(const PNGHandler&) noexcept = delete;
			PNGHandler& operator=(const PNGHandler&) noexcept = delete;
		};
	}
}

#endif