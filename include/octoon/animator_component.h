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
		~AnimatorComponent() noexcept;

		bool play(const std::string& filename) noexcept;
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
		bool _playAnimation(const std::string& filename) noexcept;
		void _updateAnimation() noexcept;
		void _destroyAnimation() noexcept;

	private:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onFrameEnd() noexcept;

	private:
		bool needUpdate_;

		bool enableAnimation_;
		bool enableAnimOnVisableOnly_;

		TimerFeature* timer_;
		animation::AnimationClips<float> clips_;

		GameObjects transforms_;
	};
}

#endif