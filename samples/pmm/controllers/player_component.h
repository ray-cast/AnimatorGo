#ifndef MYSTIC_PLAYER_COMPONENT_H_
#define MYSTIC_PLAYER_COMPONENT_H_

#include "../mysticLit_component.h"
#include "../utils/pmm.h"
#include "../module/time_module.h"

namespace MysticLit
{
	class PlayerComponent final : public MysticLitComponent<TimeModule>
	{
	public:
		PlayerComponent() noexcept;
		virtual ~PlayerComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void play() noexcept;
		void stop() noexcept;
		void render() noexcept;
	};
}

#endif