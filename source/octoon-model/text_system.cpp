#include <octoon/model/text_system.h>
#include <octoon/runtime/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace octoon
{
	namespace model
	{
		TextSystem::TextSystem() noexcept
			: library_(nullptr)
		{
		}

		TextSystem::~TextSystem() noexcept
		{
			this->close();
		}

		void
		TextSystem::setup() noexcept(false)
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
		TextSystem::close() noexcept
		{
			if (library_)
			{
				::FT_Done_FreeType((FT_Library)library_);
				library_ = nullptr;
			}
		}

		void*
		TextSystem::getLibrary() const noexcept
		{
			return library_;
		}
	}
}