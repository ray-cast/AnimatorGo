#ifndef OCTOON_IMAGEGIF_H_
#define OCTOON_IMAGEGIF_H_

#include <octoon/image/image.h>

namespace octoon
{
	namespace image
	{
		class GIFHandler final : public ImageHandler
		{
		public:
			GIFHandler() noexcept = default;
			virtual ~GIFHandler() = default;

			bool do_can_read(istream& stream) const noexcept;
			bool do_can_read(const char* type_name) const noexcept;

			bool do_load(istream& stream, Image& image) noexcept;
			bool do_save(ostream& stream, const Image& image) noexcept;

		private:
			GIFHandler(const GIFHandler&) = delete;
			GIFHandler& operator=(const GIFHandler&) = delete;
		};
	}
}

#endif