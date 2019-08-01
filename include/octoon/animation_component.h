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
		explicit AnimationComponent(animation::AnimationClip<float>&& clips) noexcept;
		explicit AnimationComponent(animation::AnimationClips<float>&& clips) noexcept;
		explicit AnimationComponent(const animation::AnimationClip<float>& clips) noexcept;
		explicit AnimationComponent(const animation::AnimationClips<float>& clips) noexcept;
		virtual ~AnimationComponent() noexcept;

		void setTimeStep(float timeStep) noexcept;
		float getTimeStep() const noexcept;

		void setTimeInterval(float timeInterval) noexcept;
		float getTimeInterval() const noexcept;

		void play() noexcept;
		void pause() noexcept;
		void stop() noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFrameBegin() except override;

	private:
		void update(float delta = 0.0f) noexcept;

	private:
		AnimationComponent(const AnimationComponent&) = delete;
		AnimationComponent& operator=(const AnimationComponent&) = delete;

	private:
		float time_;
		float timeStep_;
		float timeInterval_;

		animation::AnimationClips<float> clips_;
	};
}

#endif