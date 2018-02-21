#ifndef OCTOON_IMAGE_HDR_H_
#define OCTOON_IMAGE_HDR_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class HDRHandler final : public ImageHandler
		{
		public:
			HDRHandler() noexcept = default;
			virtual ~HDRHandler() = default;

			bool do_can_read(istream& stream) const noexcept override;
			bool do_can_read(const char* type_name) const noexcept override;

			bool do_load(istream& stream, Image& image) noexcept override;
			bool do_save(ostream& stream, const Image& image) noexcept override;

		private:
			HDRHandler(const HDRHandler&) noexcept = delete;
			HDRHandler& operator=(const HDRHandler&) noexcept = delete;
		};
	}
}

#endif