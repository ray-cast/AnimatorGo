#ifndef OCTOON_IMAGE_TGA_H_
#define OCTOON_IMAGE_TGA_H_

#include <octoon/image/image.h>

namespace octoon
{
	class TGAHandler final : public ImageLoader
	{
	public:
		TGAHandler() noexcept = default;
		virtual ~TGAHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Image& image) noexcept override;
		bool doSave(ostream& stream, const Image& image) noexcept override;

	private:
		TGAHandler(const TGAHandler&) noexcept = delete;
		TGAHandler& operator=(const TGAHandler&) noexcept = delete;
	};
}

#endif