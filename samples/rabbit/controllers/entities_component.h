#ifndef RABBIT_ENTITIES_COMPONENT_H_
#define RABBIT_ENTITIES_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/entities_module.h"

namespace rabbit
{
	class EntitiesComponent final : public RabbitComponent<EntitiesModule>
	{
	public:
		EntitiesComponent() noexcept;
		virtual ~EntitiesComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(EntitiesComponent);
		}
	};
}

#endif