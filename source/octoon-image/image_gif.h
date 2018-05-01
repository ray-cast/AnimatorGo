#ifndef OCTOON_IMAGEGIF_H_
#define OCTOON_IMAGEGIF_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class GIFHandler final : public ImageLoader
		{
		public:
			GIFHandler() noexcept = default;
			virtual ~GIFHandler() = default;

			bool doCanRead(istream& stream) const noexcept;
			bool doCanRead(const char* type_name) const noexcept;

			bool doLoad(istream& stream, Image& image) noexcept;
			bool doSave(ostream& stream, const Image& image) noexcept;

		private:
			GIFHandler(const GIFHandler&) = delete;
			GIFHandler& operator=(const GIFHandler&) = delete;
		};
	}
}

#endif