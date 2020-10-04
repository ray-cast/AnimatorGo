#ifndef OCTOON_IMAGE_ALL_H_
#define OCTOON_IMAGE_ALL_H_

#include <octoon/image/image.h>

#define OCTOON_BUILD_BMP_HANDLER  1
#define OCTOON_BUILD_DDS_HANDLER  1
#define OCTOON_BUILD_JPG_HANDLER  1
#define OCTOON_BUILD_PNG_HANDLER  1
#define OCTOON_BUILD_TGA_HANDLER  1
#define OCTOON_BUILD_HDR_HANDLER  1

namespace octoon
{
	OCTOON_EXPORT bool emptyLoader() noexcept;
	OCTOON_EXPORT bool addHandler(ImageLoaderPtr&& handler) noexcept;
	OCTOON_EXPORT bool addHandler(const ImageLoaderPtr& handler) noexcept;
	OCTOON_EXPORT bool removeHandler(const ImageLoaderPtr& handler) noexcept;

	OCTOON_EXPORT ImageLoaderPtr findHandler(const char* type) noexcept;
	OCTOON_EXPORT ImageLoaderPtr findHandler(istream& stream) noexcept;
	OCTOON_EXPORT ImageLoaderPtr findHandler(istream& stream, const char* type) noexcept;
}

#endif