#ifndef OCTOON_PMREM_LOADER_H_
#define OCTOON_PMREM_LOADER_H_

#include <octoon/texture/texture.h>

namespace octoon
{
	class OCTOON_EXPORT PMREMLoader final
	{
	public:
		static std::shared_ptr<Texture> load(std::string_view path, std::uint8_t mipNums = 8) noexcept(false);
		static std::shared_ptr<Texture> load(const std::shared_ptr<Texture>& texture, std::uint8_t mipNums = 8) noexcept(false);
	};
}

#endif