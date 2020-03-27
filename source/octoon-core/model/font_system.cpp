#include <octoon/model/font_system.h>
#include <octoon/runtime/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace octoon::font
{
	OctoonImplementSingleton(FontSystem)

	FontSystem::FontSystem() noexcept
		: library_(nullptr)
	{
	}

	FontSystem::~FontSystem() noexcept
	{
		this->close();
	}

	void
	FontSystem::setup() noexcept(false)
	{
		if (library_ == nullptr)
		{
			FT_Library library = nullptr;

			if (::FT_Init_FreeType(&library))
				throw runtime::runtime_error::create("FT_Init_FreeType() failed", 1);

			library_ = library;
		}
	}

	void
	FontSystem::close() noexcept
	{
		if (library_)
		{
			::FT_Done_FreeType((FT_Library)library_);
			library_ = nullptr;
		}
	}

	void*
	FontSystem::getLibrary() const noexcept
	{
		return library_;
	}
}