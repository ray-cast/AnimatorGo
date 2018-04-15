#ifndef OCTOON_TEXT_FILE_H_
#define OCTOON_TEXT_FILE_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT TextFile final
		{
		public:
			TextFile() noexcept;
			TextFile(const char* fontpath) noexcept(false);
			TextFile(const std::uint8_t* stream, std::size_t size) noexcept(false);
			~TextFile() noexcept;

			void open(const char* fontpath) noexcept(false);
			void open(const std::uint8_t* stream, std::size_t size) noexcept(false);
			void close() noexcept;

			const std::string& getFontPath() const noexcept;

		private:
			friend class FontTextMeshing;
			void* getFont() const noexcept;

		private:
			TextFile(const TextFile&) = delete;
			TextFile& operator=(const TextFile&) = delete;

		private:
			void* font_;
			std::string fontpath_;
		};
	}
}

#endif