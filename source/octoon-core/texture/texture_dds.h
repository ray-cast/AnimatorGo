#ifndef OCTOON_TEXTURE_DDS_H_
#define OCTOON_TEXTURE_DDS_H_

#include <octoon/texture/texture.h>

namespace octoon
{
	class DDSHandler final : public TextureHandler
	{
	public:
		DDSHandler() noexcept = default;
		virtual ~DDSHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Texture& image) noexcept override;
		bool doSave(ostream& stream, const Texture& image) noexcept override;

	private:
		DDSHandler(const DDSHandler&) noexcept = delete;
		DDSHandler& operator=(const DDSHandler&) noexcept = delete;
	};
}

#endif