#ifndef UNREAL_PLAYER_COMPONENT_H_
#define UNREAL_PLAYER_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/player_module.h"
#include <octoon/runtime/timer.h>

namespace unreal
{
	class PlayerComponent final : public UnrealComponent<PlayerModule>
	{
	public:
		PlayerComponent() noexcept;
		virtual ~PlayerComponent() noexcept;

		void play() noexcept;
		void pause() noexcept;
		void render() noexcept;
		void reset() noexcept;
		
		void sample(float delta = 0.0f) noexcept;
		void evaluate(float delta) noexcept;

		void updateTimeLength() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(PlayerComponent);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onFixedUpdate() noexcept override;
		void onLateUpdate() noexcept override;

	private:
		void updateDofTarget() noexcept;

	private:
		bool needAnimationEvaluate_;

		octoon::Timer timer_;
	};
}

#endif