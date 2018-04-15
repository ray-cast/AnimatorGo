#include <octoon/video/text_file.h>
#include <octoon/video/text_system.h>
#include <octoon/runtime/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace octoon
{
	namespace video
	{
		TextFile::TextFile() noexcept
			: font_(nullptr)
		{
		}

		TextFile::TextFile(const char* fontpath) noexcept(false)
			: font_(nullptr)
		{
			this->open(fontpath);
		}

		TextFile::TextFile(const std::uint8_t* stream, std::size_t size) noexcept(false)
			: font_(nullptr)
		{
			this->open(stream, size);
		}

		TextFile::~TextFile() noexcept
		{
			this->close();
		}

		void
		TextFile::open(const char* fontpath) noexcept(false)
		{
			assert(fontpath);

			if (font_ != nullptr)
				this->close();

			FT_Face face = nullptr;
			FT_Library library = (FT_Library)TextSystem::instance()->getLibrary();

			if (::FT_New_Face(library, fontpath, 0, &face))
				throw runtime::runtime_error::create("FT_New_Face() failed (there is probably a problem with your font file.", 2);

			::FT_Select_Charmap(face, FT_ENCODING_UNICODE);

			font_ = face;
			fontpath_ = fontpath;
		}

		void
		TextFile::open(const std::uint8_t* stream, std::size_t size) noexcept(false)
		{
			assert(stream);

			if (font_ != nullptr)
				this->close();

			FT_Face face = nullptr;
			FT_Library library = (FT_Library)TextSystem::instance()->getLibrary();

			if (::FT_New_Memory_Face(library, stream, size, 0, &face))
				throw runtime::runtime_error::create("FT_New_Memory_Face() failed (there is probably a problem with your stream.", 2);

			::FT_Select_Charmap(face, FT_ENCODING_UNICODE);

			font_ = face;
		}

		void
		TextFile::close() noexcept
		{
			if (font_)
			{
				::FT_Done_Face((FT_Face)font_);
				font_ = nullptr;
			}

			fontpath_.clear();
		}

		const std::string&
		TextFile::getFontPath() const noexcept
		{
			return fontpath_;
		}

		void*
		TextFile::getFont() const noexcept
		{
			return font_;
		}
	}
}