#ifndef OCTOON_IMAGE_DDS_H_
#define OCTOON_IMAGE_DDS_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class DDSHandler final : public ImageHandler
		{
		public:
			DDSHandler() noexcept = default;
			virtual ~DDSHandler() = default;

			bool do_can_read(istream& stream) const noexcept override;
			bool do_can_read(const char* type_name) const noexcept override;

			bool do_load(istream& stream, Image& image) noexcept override;
			bool do_save(ostream& stream, const Image& image) noexcept override;

		private:
			DDSHandler(const DDSHandler&) noexcept = delete;
			DDSHandler& operator=(const DDSHandler&) noexcept = delete;
		};
	}
}

#endif