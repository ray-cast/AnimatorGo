#ifndef UNREAL_LIGHT_COMPONENT_H_
#define UNREAL_LIGHT_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/entities_module.h"

namespace unreal
{
	enum LightType
	{
		Point = 0,
		Spot = 1,
		Directional = 2
	};

	class LightComponent final : public UnrealComponent<EntitiesModule>
	{
	public:
		LightComponent() noexcept;
		virtual ~LightComponent() noexcept;

		bool createLight(LightType) noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(LightComponent);
		}

	private:
		LightComponent(const LightComponent&) = delete;
		LightComponent& operator=(const LightComponent&) = delete;
	};
}

#endif