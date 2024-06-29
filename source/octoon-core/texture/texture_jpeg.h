#ifndef OCTOON_TEXTURE_JPEG_H_
#define OCTOON_TEXTURE_JPEG_H_

#include <octoon/texture/texture.h>

namespace octoon
{
	class JPEGHandler final : public TextureHandler
	{
	public:
		JPEGHandler() noexcept = default;
		virtual ~JPEGHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Texture& image) noexcept override;
		bool doSave(ostream& stream, const Texture& image) noexcept override;

	private:
		JPEGHandler(const JPEGHandler&) noexcept = delete;
		JPEGHandler& operator=(const JPEGHandler&) noexcept = delete;
	};
}

#endif