#ifndef OCTOON_TEXTURE_HDR_H_
#define OCTOON_TEXTURE_HDR_H_

#include <octoon/texture/texture.h>

namespace octoon
{
	class HDRHandler final : public TextureHandler
	{
	public:
		HDRHandler() noexcept = default;
		virtual ~HDRHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Texture& image) noexcept override;
		bool doSave(ostream& stream, const Texture& image) noexcept override;

	private:
		HDRHandler(const HDRHandler&) noexcept = delete;
		HDRHandler& operator=(const HDRHandler&) noexcept = delete;
	};
}

#endif