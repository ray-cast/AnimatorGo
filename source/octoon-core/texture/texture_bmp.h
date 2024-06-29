#ifndef OCTOON_TEXTURE_BMP_H_
#define OCTOON_TEXTURE_BMP_H_

#include <octoon/texture/texture.h>

namespace octoon
{
	struct BITMAPINFO;

	class BMPHandler final : public TextureHandler
	{
	public:
		BMPHandler() noexcept = default;
		virtual ~BMPHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Texture& image) noexcept(false) override;
		bool doSave(ostream& stream, const Texture& image) noexcept(false) override;

	private:
		bool decode(istream& stream, Texture& image, const BITMAPINFO& info);
		bool encode(istream& stream, Texture& image, const BITMAPINFO& info);
		bool loadDIB(istream& stream, Texture& image, const BITMAPINFO& info);
	};
}

#endif