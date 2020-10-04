#ifndef OCTOON_PMREM_LOADER_H_
#define OCTOON_PMREM_LOADER_H_

#include <octoon/hal/graphics_types.h>

namespace octoon
{
	class OCTOON_EXPORT PMREMLoader final
	{
	public:
		static hal::GraphicsTexturePtr load(std::string_view path, std::uint8_t mipNums = 8, bool cache = true) noexcept(false);
		static hal::GraphicsTexturePtr load(const hal::GraphicsTexturePtr& texture, std::uint8_t mipNums = 8, bool cache = true) noexcept(false);
	};
}

#endif