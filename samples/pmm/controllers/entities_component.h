#ifndef MYSTIC_ENTITIES_COMPONENT_H_
#define MYSTIC_ENTITIES_COMPONENT_H_

#include "../mysticLit_component.h"
#include "../module/entities_module.h"

namespace MysticLit
{
	class EntitiesComponent final : public MysticLitComponent<EntitiesModule>
	{
	public:
		EntitiesComponent() noexcept;
		virtual ~EntitiesComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void play() noexcept;
		void stop() noexcept;
	};
}

#endif