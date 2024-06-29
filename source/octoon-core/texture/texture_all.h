#ifndef OCTOON_TEXTURE_ALL_H_
#define OCTOON_TEXTURE_ALL_H_

#include <octoon/texture/texture.h>

#define OCTOON_BUILD_BMP_HANDLER  1
#define OCTOON_BUILD_DDS_HANDLER  1
#define OCTOON_BUILD_JPG_HANDLER  1
#define OCTOON_BUILD_PNG_HANDLER  1
#define OCTOON_BUILD_TGA_HANDLER  1
#define OCTOON_BUILD_HDR_HANDLER  1

namespace octoon
{
	OCTOON_EXPORT bool emptyLoader() noexcept;
	OCTOON_EXPORT bool addHandler(std::shared_ptr<TextureHandler>&& handler) noexcept;
	OCTOON_EXPORT bool addHandler(const std::shared_ptr<TextureHandler>& handler) noexcept;
	OCTOON_EXPORT bool removeHandler(const std::shared_ptr<TextureHandler>& handler) noexcept;

	OCTOON_EXPORT std::shared_ptr<TextureHandler> findHandler(const char* type) noexcept;
	OCTOON_EXPORT std::shared_ptr<TextureHandler> findHandler(istream& stream) noexcept;
	OCTOON_EXPORT std::shared_ptr<TextureHandler> findHandler(istream& stream, const char* type) noexcept;
}

#endif