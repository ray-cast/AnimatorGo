#ifndef MYSTIC_PLAYER_COMPONENT_H_
#define MYSTIC_PLAYER_COMPONENT_H_

#include "../mysticLit_component.h"
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
		void pause() noexcept;
		void reset() noexcept;

		void render() noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;
	};
}

#endif