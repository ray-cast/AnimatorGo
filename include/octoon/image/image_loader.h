#ifndef OCTOON_IMAGE_LOADER_H_
#define OCTOON_IMAGE_LOADER_H_

#include <octoon/image/image_types.h>

namespace octoon
{
	class OCTOON_EXPORT ImageLoader
	{
	public:
		ImageLoader() noexcept = default;
		virtual ~ImageLoader() = default;

		virtual bool doCanRead(const char* type_name) const noexcept = 0;
		virtual bool doCanRead(istream& stream) const noexcept = 0;

		virtual bool doLoad(istream& stream, Image& image) except = 0;
		virtual bool doSave(ostream& stream, const Image& image) except = 0;

	private:
		ImageLoader(const ImageLoader&) noexcept = delete;
		const ImageLoader& operator=(const ImageLoader&) noexcept = delete;
	};
}

#endif