#ifndef RABBIT_PLAYER_COMPONENT_H_
#define RABBIT_PLAYER_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/time_module.h"

namespace rabbit
{
	class PlayerComponent final : public RabbitComponent<TimeModule>
	{
	public:
		PlayerComponent() noexcept;
		virtual ~PlayerComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void play() noexcept;
		void stop() noexcept;
		void reset() noexcept;
		
		void sample(float delta = 0.0f) noexcept;
		void evaluate(float delta) noexcept;

		float time() const noexcept;
		float timeLength() const noexcept;

		void render() noexcept;

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
		bool needUpdate_;

		float time_;
		float timeStep_;
		float timeInterval_;

		std::uint32_t timeCount_;
	};
}

#endif