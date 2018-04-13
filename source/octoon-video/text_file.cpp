#include <octoon/video/text_file.h>
#include <octoon/video/text_system.h>
#include <octoon/runtime/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace octoon
{
	namespace render
	{
		FontFile::FontFile() noexcept
			: font_(nullptr)
		{
		}

		FontFile::FontFile(const char* fontpath) noexcept(false)
			: font_(nullptr)
		{
			this->open(fontpath);
		}

		FontFile::FontFile(const std::uint8_t* stream, std::size_t size) noexcept(false)
			: font_(nullptr)
		{
			this->open(stream, size);
		}

		FontFile::~FontFile() noexcept
		{
			this->close();
		}

		void
		FontFile::open(const char* fontpath) noexcept(false)
		{
			assert(fontpath);

			if (font_ != nullptr)
				this->close();

			FT_Face face = nullptr;
			FT_Library library = (FT_Library)FontSystem::instance()->getLibrary();

			if (::FT_New_Face(library, fontpath, 0, &face))
				throw runtime::runtime_error::create("FT_New_Face() failed (there is probably a problem with your font file.", 2);

			::FT_Select_Charmap(face, FT_ENCODING_UNICODE);

			font_ = face;
			fontpath_ = fontpath;
		}

		void
		FontFile::open(const std::uint8_t* stream, std::size_t size) noexcept(false)
		{
			assert(stream);

			if (font_ != nullptr)
				this->close();

			FT_Face face = nullptr;
			FT_Library library = (FT_Library)FontSystem::instance()->getLibrary();

			if (::FT_New_Memory_Face(library, stream, size, 0, &face))
				throw runtime::runtime_error::create("FT_New_Memory_Face() failed (there is probably a problem with your stream.", 2);

			::FT_Select_Charmap(face, FT_ENCODING_UNICODE);

			font_ = face;
		}

		void
		FontFile::close() noexcept
		{
			if (font_)
			{
				::FT_Done_Face((FT_Face)font_);
				font_ = nullptr;
			}

			fontpath_.clear();
		}

		const std::string&
		FontFile::getFontPath() const noexcept
		{
			return fontpath_;
		}

		void*
		FontFile::getFont() const noexcept
		{
			return font_;
		}
	}
}