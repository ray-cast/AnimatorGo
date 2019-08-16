#ifndef OCTOON_ANIMATION_COMPONENT_H_
#define OCTOON_ANIMATION_COMPONENT_H_

#include <octoon/animation/animation.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT AnimationComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AnimationComponent, GameComponent)
	public:
		AnimationComponent() noexcept;
		explicit AnimationComponent(animation::Animation<float>&& animation) noexcept;
		explicit AnimationComponent(const animation::Animation<float>& animation) noexcept;
		virtual ~AnimationComponent() noexcept;
	
		void play() noexcept;
		void pause() noexcept;
		void stop() noexcept;

		void setTime(float time) noexcept;

		void setAnimation(animation::Animation<float>&& animation) noexcept;
		void setAnimation(const animation::Animation<float>& animation) noexcept;
		const animation::Animation<float>& getAnimation() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() except override;

	private:
		void update(float delta = 0.0f) noexcept;

	private:
		AnimationComponent(const AnimationComponent&) = delete;
		AnimationComponent& operator=(const AnimationComponent&) = delete;

	private:
		float time_;
		float timeStep_;
		float timeInterval_;

		animation::Animation<float> animation_;
	};
}

#endif