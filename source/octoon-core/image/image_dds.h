#ifndef OCTOON_IMAGE_DDS_H_
#define OCTOON_IMAGE_DDS_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class DDSHandler final : public ImageLoader
		{
		public:
			DDSHandler() noexcept = default;
			virtual ~DDSHandler() = default;

			bool doCanRead(istream& stream) const noexcept override;
			bool doCanRead(const char* type_name) const noexcept override;

			bool doLoad(istream& stream, Image& image) noexcept override;
			bool doSave(ostream& stream, const Image& image) noexcept override;

		private:
			DDSHandler(const DDSHandler&) noexcept = delete;
			DDSHandler& operator=(const DDSHandler&) noexcept = delete;
		};
	}
}

#endif