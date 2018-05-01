#ifndef OCTOON_IMAGE_H_
#define OCTOON_IMAGE_H_

#include <octoon/image/image_types.h>
#include <octoon/image/image_format.h>
#include <octoon/image/image_loader.h>

namespace octoon
{
	namespace image
	{
		class OCTOON_EXPORT Image final
		{
		public:
			Image() noexcept;
			Image(Image&& move) noexcept;
			Image(const Image& move) noexcept;
			Image(Format format, std::uint32_t width, std::uint32_t height) except;
			Image(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth) except;
			Image(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase = 0, std::uint32_t layerBase = 0) except;
			Image(Format format, const Image& src) except;
			~Image() noexcept;

			bool create(Format format, std::uint32_t width, std::uint32_t height) except;
			bool create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth) except;
			bool create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase = 0, std::uint32_t layerBase = 0) except;
			bool create(Format format, const Image& src) except;

			bool empty() const noexcept;

			const Format& format() const noexcept;

			std::size_t size() const noexcept;

			std::uint32_t width() const noexcept;
			std::uint32_t height() const noexcept;
			std::uint32_t depth() const noexcept;

			std::uint32_t mipBase() const noexcept;
			std::uint32_t mipLevel() const noexcept;

			std::uint32_t layerBase() const noexcept;
			std::uint32_t layerLevel() const noexcept;

			const char* data() const noexcept;

		public:
			bool load(istream& stream, const char* type = nullptr) noexcept;
			bool save(ostream& stream, const char* type = "tga") noexcept;

		private:
			Format format_;

			std::uint32_t width_;
			std::uint32_t height_;
			std::uint32_t depth_;

			std::uint32_t mipBase_;
			std::uint32_t mipLevel_;

			std::uint32_t layerBase_;
			std::uint32_t layerLevel_;

			std::vector<char> data_;
		};
	}
}

#endif