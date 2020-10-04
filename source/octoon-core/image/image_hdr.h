#ifndef OCTOON_IMAGE_HDR_H_
#define OCTOON_IMAGE_HDR_H_

#include <octoon/image/image.h>

namespace octoon
{
	class HDRHandler final : public ImageLoader
	{
	public:
		HDRHandler() noexcept = default;
		virtual ~HDRHandler() = default;

		bool doCanRead(istream& stream) const noexcept override;
		bool doCanRead(const char* type_name) const noexcept override;

		bool doLoad(istream& stream, Image& image) noexcept override;
		bool doSave(ostream& stream, const Image& image) noexcept override;

	private:
		HDRHandler(const HDRHandler&) noexcept = delete;
		HDRHandler& operator=(const HDRHandler&) noexcept = delete;
	};
}

#endif