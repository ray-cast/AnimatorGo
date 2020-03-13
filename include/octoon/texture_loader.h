#ifndef OCTOON_TEXTURE_LOADER_H_
#define OCTOON_TEXTURE_LOADER_H_

#include <octoon/hal/graphics_types.h>

namespace octoon
{
	class OCTOON_EXPORT TextureLoader final
	{
	public:
		static hal::GraphicsTexturePtr load(const std::string_view& path, bool cache = true) noexcept(false);
	};
}

#endif