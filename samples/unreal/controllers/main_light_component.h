#ifndef UNREAL_MAIN_LIGHT_COMPONENT_H_
#define UNREAL_MAIN_LIGHT_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/main_light_module.h"

namespace unreal
{
	class MainLightComponent final : public UnrealComponent<MainLightModule>
	{
	public:
		MainLightComponent() noexcept;
		virtual ~MainLightComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(MainLightComponent);
		}

	private:
		void onInit() noexcept override;

		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		MainLightComponent(const MainLightComponent&) = delete;
		MainLightComponent& operator=(const MainLightComponent&) = delete;
	};
}

#endif