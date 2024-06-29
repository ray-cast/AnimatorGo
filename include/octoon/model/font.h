#ifndef OCTOON_FONT_H_
#define OCTOON_FONT_H_

#include <string>
#include <octoon/runtime/platform.h>

namespace octoon::font
{
	class OCTOON_EXPORT Font final
	{
	public:
		Font() noexcept;
		Font(const std::uint8_t* stream, std::uint32_t size) noexcept(false);
		explicit Font(const char* fontpath) noexcept(false);
		~Font() noexcept;

		void open(const char* fontpath) noexcept(false);
		void open(const std::uint8_t* stream, std::uint32_t size) noexcept(false);
		void close() noexcept;

		const std::string& getFontPath() const noexcept;

		void* getFont() const noexcept;

	private:
		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;

	private:
		void* font_;
		std::string fontpath_;
	};
}

#endif