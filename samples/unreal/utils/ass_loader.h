#ifndef UNREAL_ASS_LOADER_H_
#define UNREAL_ASS_LOADER_H_

#include "../unreal_profile.h"
#include "../module/sound_module.h"
#include "../module/entities_module.h"

#include <optional>
#include <octoon/octoon.h>

namespace unreal
{
	class AssLoader final
	{
	public:
		AssLoader() noexcept;
		virtual ~AssLoader() noexcept;

		static void load(UnrealProfile& profile, const std::filesystem::path& path) noexcept(false);
	};
}

#endif