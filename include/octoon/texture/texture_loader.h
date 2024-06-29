#ifndef OCTOON_TEXTURE_HANDLER_H_
#define OCTOON_TEXTURE_HANDLER_H_

#include <octoon/texture/texture_types.h>

namespace octoon
{
	class Texture;
	class OCTOON_EXPORT TextureHandler
	{
	public:
		TextureHandler() noexcept = default;
		virtual ~TextureHandler() = default;

		virtual bool doCanRead(const char* type_name) const noexcept = 0;
		virtual bool doCanRead(istream& stream) const noexcept = 0;

		virtual bool doLoad(istream& stream, Texture& image) noexcept(false) = 0;
		virtual bool doSave(ostream& stream, const Texture& image) noexcept(false) = 0;

	private:
		TextureHandler(const TextureHandler&) noexcept = delete;
		const TextureHandler& operator=(const TextureHandler&) noexcept = delete;
	};
}

#endif