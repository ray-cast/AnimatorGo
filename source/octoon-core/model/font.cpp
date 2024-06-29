#include <octoon/model/font.h>
#include <octoon/model/font_system.h>
#include <octoon/runtime/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace octoon::font
{
	Font::Font() noexcept
		: font_(nullptr)
	{
	}

	Font::Font(const char* fontpath) noexcept(false)
		: font_(nullptr)
	{
		this->open(fontpath);
	}

	Font::Font(const std::uint8_t* stream, std::uint32_t size) noexcept(false)
		: font_(nullptr)
	{
		this->open(stream, size);
	}

	Font::~Font() noexcept
	{
		this->close();
	}

	void
	Font::open(const char* fontpath) noexcept(false)
	{
		assert(fontpath);

		if (font_ != nullptr)
			this->close();

		FT_Face face = nullptr;
		FT_Library library = (FT_Library)FontSystem::instance()->getLibrary();

		if (::FT_New_Face(library, fontpath, 0, &face))
			throw runtime_error::create("FT_New_Face() failed (there is probably a problem with your font file.", 2);

		::FT_Select_Charmap(face, FT_ENCODING_UNICODE);

		font_ = face;
		fontpath_ = fontpath;
	}

	void
	Font::open(const std::uint8_t* stream, std::uint32_t size) noexcept(false)
	{
		assert(stream);

		if (font_ != nullptr)
			this->close();

		FT_Face face = nullptr;
		FT_Library library = (FT_Library)FontSystem::instance()->getLibrary();

		if (::FT_New_Memory_Face(library, stream, size, 0, &face))
			throw runtime_error::create("FT_New_Memory_Face() failed (there is probably a problem with your stream.", 2);

		::FT_Select_Charmap(face, FT_ENCODING_UNICODE);

		font_ = face;
	}

	void
	Font::close() noexcept
	{
		if (font_)
		{
			::FT_Done_Face((FT_Face)font_);
			font_ = nullptr;
		}

		fontpath_.clear();
	}

	const std::string&
	Font::getFontPath() const noexcept
	{
		return fontpath_;
	}

	void*
	Font::getFont() const noexcept
	{
		return font_;
	}
}