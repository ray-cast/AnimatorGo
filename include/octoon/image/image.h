#ifndef OCTOON_IMAGE_H_
#define OCTOON_IMAGE_H_

#include <octoon/image/image_format.h>
#include <octoon/image/image_loader.h>

namespace octoon
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
		explicit Image(istream& stream, const char* type = nullptr) noexcept;
		explicit Image(const char* filepath, const char* type = nullptr) noexcept;
		explicit Image(const std::string& filepath, const char* type = nullptr) noexcept;
		~Image() noexcept;

		bool create(Format format, std::uint32_t width, std::uint32_t height) except;
		bool create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth) except;
		bool create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase = 0, std::uint32_t layerBase = 0) except;

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

		const std::uint8_t* data() const noexcept;
		const std::uint8_t* data(std::size_t i) const noexcept;

		Image convert(Format format) noexcept(false);

	public:
		bool load(istream& stream, const char* type = nullptr) noexcept;
		bool load(const char* filepath, const char* type = nullptr) noexcept;
		bool load(const std::string& filepath, const char* type = nullptr) noexcept;

		bool save(ostream& stream, const char* type = "tga") noexcept;
		bool save(const char* filepath, const char* type = "tga") noexcept;
		bool save(const std::string& filepath, const char* type = "tga") noexcept;

	private:
		Format format_;

		std::uint32_t width_;
		std::uint32_t height_;
		std::uint32_t depth_;

		std::uint32_t mipBase_;
		std::uint32_t mipLevel_;

		std::uint32_t layerBase_;
		std::uint32_t layerLevel_;

		std::vector<std::uint8_t> data_;
	};
}

#endif