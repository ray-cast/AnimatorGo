#ifndef UNREAL_SOUND_COMPONENT_H_
#define UNREAL_SOUND_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/sound_module.h"

namespace unreal
{
	class SoundComponent final : public UnrealComponent<SoundModule>
	{
	public:
		SoundComponent() noexcept;
		virtual ~SoundComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(SoundComponent);
		}

	private:
		void onInit() noexcept override;

		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		SoundComponent(const SoundComponent&) = delete;
		SoundComponent& operator=(const SoundComponent&) = delete;
	};
}

#endif