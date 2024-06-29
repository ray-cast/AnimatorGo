#ifndef OCTOON_TEXTURE_PNG_H_
#define OCTOON_TEXTURE_PNG_H_

#include <octoon/texture/texture.h>

namespace octoon
{
	class PNGHandler final : public TextureHandler
	{
	public:
		PNGHandler() noexcept = default;
		virtual ~PNGHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Texture& image) noexcept override;
		bool doSave(ostream& stream, const Texture& image) noexcept override;

	private:
		PNGHandler(const PNGHandler&) noexcept = delete;
		PNGHandler& operator=(const PNGHandler&) noexcept = delete;
	};
}

#endif