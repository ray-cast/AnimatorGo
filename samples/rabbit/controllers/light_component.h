#ifndef RABBIT_LIGHT_COMPONENT_H_
#define RABBIT_LIGHT_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/entities_module.h"

namespace rabbit
{
	enum LightType
	{
		Point = 0,
		Spot = 1,
		Directional = 2
	};

	class LightComponent final : public RabbitComponent<EntitiesModule>
	{
	public:
		LightComponent() noexcept;
		virtual ~LightComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

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