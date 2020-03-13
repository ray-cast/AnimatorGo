#ifndef OCTOON_TEXT_HELPER_H_
#define OCTOON_TEXT_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT TextHelper final
	{
	public:
		static GameObjectPtr create(const wchar_t* text, std::uint16_t fontsize = 24, const char* fontPath = "../../system/fonts/DroidSansFallback.ttf") noexcept;
		static GameObjectPtr create(const char* u8str, std::uint16_t fontsize = 24, const char* fontPath = "../../system/fonts/DroidSansFallback.ttf") noexcept;
	};
}

#endif