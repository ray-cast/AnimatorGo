#ifndef OCTOON_ANIMATOR_COMPONENT_H_
#define OCTOON_ANIMATOR_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/timer_feature.h>
#include <octoon/animation/animation.h>

namespace octoon
{
	class OCTOON_EXPORT AnimatorComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AnimatorComponent, GameComponent)
	public:
		AnimatorComponent() noexcept;
		explicit AnimatorComponent(animation::AnimationClip<float>&& clips) noexcept;
		explicit AnimatorComponent(animation::AnimationClips<float>&& clips) noexcept;
		explicit AnimatorComponent(const animation::AnimationClip<float>& clips) noexcept;
		explicit AnimatorComponent(const animation::AnimationClips<float>& clips) noexcept;
		explicit AnimatorComponent(GameObjects&& transforms) noexcept;
		explicit AnimatorComponent(const GameObjects& transforms) noexcept;
		~AnimatorComponent() noexcept;

		void setTimeStep(float timeStep) noexcept;
		float getTimeStep() const noexcept;

		bool play(const std::string& status = "default") noexcept;
		void pause() noexcept;
		void stop() noexcept;

		void setClips(animation::AnimationClips<float>&& transforms) noexcept;
		void setClips(const animation::AnimationClips<float>& transforms) noexcept;
		const animation::AnimationClips<float>& getClips() const noexcept;

		void setTransforms(GameObjects&& transforms) noexcept;
		void setTransforms(const GameObjects& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		GameComponentPtr clone() const noexcept;

	private:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onFrameEnd() noexcept;

	private:
		void updateBindpose(const GameObjects& transforms) noexcept;
		void updateBones() noexcept;

	private:
		bool enableAnimation_;
		bool enableAnimOnVisableOnly_;

		float time_;
		float timeStep_;

		TimerFeature* timer_;
		animation::AnimationClips<float> clips_;
		math::float3s bindpose_;

		GameObjects bones_;
	};
}

#endif