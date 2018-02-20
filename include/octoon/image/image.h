#ifndef OCTOON_IMAGE_H_
#define OCTOON_IMAGE_H_

#include <octoon/image/image_types.h>
#include <octoon/image/image_handler.h>

namespace octoon
{
	namespace image
	{
		class OCTOON_EXPORT Image final
		{
		public:
			Image() noexcept;
			Image(Image&& move) noexcept;
			~Image() noexcept;

			bool create(std::uint32_t width, std::uint32_t height, Format format, bool clear = true) noexcept;
			bool create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, Format format, bool clear = true) noexcept;
			bool create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, Format format, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase = 0, std::uint32_t layerBase = 0, bool clear = true) noexcept;
			bool create(const Image& src, Format format = Format::Undefined) noexcept;

			void clear() noexcept;
			bool empty() const noexcept;

			Format format() const noexcept;

			std::uint32_t width() const noexcept;
			std::uint32_t height() const noexcept;
			std::uint32_t depth() const noexcept;

			std::size_t size() const noexcept;

			std::uint32_t mipBase() const noexcept;
			std::uint32_t mipLevel() const noexcept;

			std::uint32_t layerBase() const noexcept;
			std::uint32_t layerLevel() const noexcept;

			const char* data() const noexcept;

			value_t value_type() const noexcept;
			swizzle_t swizzle_type() const noexcept;

			std::uint8_t channel() const noexcept;
			std::uint8_t type_size() const noexcept;

		public:
			static value_t value_type(Format format) noexcept;
			static swizzle_t swizzle_type(Format format) noexcept;
			static std::uint8_t channel(Format format) noexcept;
			static std::uint8_t type_size(Format format) noexcept;

		public:
			bool load(istream& stream, const char* type = nullptr) noexcept;
			bool save(ostream& stream, const char* type = "tga") noexcept;

		private:
			void _init() noexcept;

		private:
			Image(const Image&) noexcept = delete;
			Image& operator=(const Image&) noexcept = delete;

		private:
			Format format_;

			std::uint32_t width_;
			std::uint32_t height_;
			std::uint32_t depth_;

			std::uint32_t mipBase_;
			std::uint32_t mipLevel_;

			std::uint32_t layerBase_;
			std::uint32_t layerLevel_;

			std::size_t size_;

			std::unique_ptr<std::uint8_t[]> data_;
		};
	}
}

#endif