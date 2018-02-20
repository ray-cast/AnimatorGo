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

			bool do_can_read(istream& stream) const noexcept;
			bool do_can_read(const char* type_name) const noexcept;

			bool do_load(istream& stream, Image& image) noexcept;
			bool do_save(ostream& stream, const Image& image) noexcept;

		private:
			DDSHandler(const DDSHandler&) noexcept = delete;
			DDSHandler& operator=(const DDSHandler&) noexcept = delete;
		};
	}
}

#endif