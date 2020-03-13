#ifndef OCTOON_IMAGE_PNG_H_
#define OCTOON_IMAGE_PNG_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class PNGHandler final : public ImageLoader
		{
		public:
			PNGHandler() noexcept = default;
			virtual ~PNGHandler() = default;

			bool doCanRead(istream& stream) const noexcept override;
			bool doCanRead(const char* type_name) const noexcept override;

			bool doLoad(istream& stream, Image& image) noexcept override;
			bool doSave(ostream& stream, const Image& image) noexcept override;

		private:
			PNGHandler(const PNGHandler&) noexcept = delete;
			PNGHandler& operator=(const PNGHandler&) noexcept = delete;
		};
	}
}

#endif