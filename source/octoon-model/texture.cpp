#include <octoon/model/texture.h>

namespace octoon
{
	namespace model
	{
		Texture::Texture() noexcept
		{
		}

		Texture::Texture(std::string&& filename) noexcept
			: name(std::move(filename))
		{
		}

		Texture::Texture(const std::string& filename) noexcept
			: name(filename)
		{
		}

		Texture::Texture(const char* filename, std::size_t length) noexcept
			: name(filename, length)
		{
		}
	}
}