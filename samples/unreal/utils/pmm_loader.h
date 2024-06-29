#ifndef UNREAL_PMM_LOADER_H_
#define UNREAL_PMM_LOADER_H_

#include "../utils/pmm.h"
#include "../unreal_profile.h"
#include "../module/sound_module.h"
#include "../module/entities_module.h"

#include <optional>
#include <octoon/octoon.h>

namespace unreal
{
	class PMMLoader final
	{
	public:
		PMMLoader() noexcept;
		virtual ~PMMLoader() noexcept;

		static void load(UnrealProfile& profile, const std::filesystem::path& path) noexcept(false);

	private:
		static void setupBoneAnimation(const octoon::PmmModel& model, octoon::AnimationClip& clip) noexcept;
		static void setupMorphAnimation(const octoon::PmmModel& model, octoon::AnimationClip& clip) noexcept;
		static void setupCameraAnimation(const std::vector<octoon::PmmKeyframeCamera>& camera, octoon::Animation& animtion) noexcept;
	};
}

#endif