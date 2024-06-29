#ifndef OCTOON_TEXTURE_TGA_H_
#define OCTOON_TEXTURE_TGA_H_

#include <octoon/texture/texture.h>

namespace octoon
{
	class TGAHandler final : public TextureHandler
	{
	public:
		TGAHandler() noexcept = default;
		virtual ~TGAHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Texture& image) noexcept override;
		bool doSave(ostream& stream, const Texture& image) noexcept override;

	private:
		TGAHandler(const TGAHandler&) noexcept = delete;
		TGAHandler& operator=(const TGAHandler&) noexcept = delete;
	};
}

#endif