#ifndef FLOWER_PLAYER_COMPONENT_H_
#define FLOWER_PLAYER_COMPONENT_H_

#include "../flower_component.h"
#include "../module/player_module.h"

namespace flower
{
	class PlayerComponent final : public RabbitComponent<PlayerModule>
	{
	public:
		PlayerComponent() noexcept;
		virtual ~PlayerComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void play() noexcept;
		void pause() noexcept;
		void render() noexcept;
		void reset() noexcept;
		
		void sample(float delta = 0.0f) noexcept;
		void evaluate(float delta) noexcept;

		float timeLength() const noexcept;

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

		float time_;
	};
}

#endif