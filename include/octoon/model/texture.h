#ifndef OCTOON_MODEL_TEXTURE_H_
#define OCTOON_MODEL_TEXTURE_H_

#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Texture final
		{
		public:
			Texture() noexcept;
			Texture(std::string&& filename) noexcept;
			Texture(const std::string& filename) noexcept;
			Texture(const char* filename, std::size_t length) noexcept;

			std::string   name;
			std::uint32_t width;
			std::uint32_t height;
		};
	}
}

#endif