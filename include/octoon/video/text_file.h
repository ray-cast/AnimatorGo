#ifndef OCTOON_TEXT_FILE_H_
#define OCTOON_TEXT_FILE_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	namespace render
	{
		class OCTOON_EXPORT FontFile final
		{
		public:
			FontFile() noexcept;
			FontFile(const char* fontpath) noexcept(false);
			FontFile(const std::uint8_t* stream, std::size_t size) noexcept(false);
			~FontFile() noexcept;

			void open(const char* fontpath) noexcept(false);
			void open(const std::uint8_t* stream, std::size_t size) noexcept(false);
			void close() noexcept;

			const std::string& getFontPath() const noexcept;

		private:
			friend class FontTextMeshing;
			void* getFont() const noexcept;

		private:
			FontFile(const FontFile&) = delete;
			FontFile& operator=(const FontFile&) = delete;

		private:
			void* font_;
			std::string fontpath_;
		};
	}
}

#endif