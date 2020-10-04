#ifndef OCTOON_IMAGE_BMP_H_
#define OCTOON_IMAGE_BMP_H_

#include <octoon/image/image.h>

namespace octoon
{
	struct BITMAPINFO;

	class BMPHandler final : public ImageLoader
	{
	public:
		BMPHandler() noexcept = default;
		virtual ~BMPHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Image& image) except override;
		bool doSave(ostream& stream, const Image& image) except override;

	private:
		bool decode(istream& stream, Image& image, const BITMAPINFO& info);
		bool encode(istream& stream, Image& image, const BITMAPINFO& info);
		bool loadDIB(istream& stream, Image& image, const BITMAPINFO& info);
	};
}

#endif