#ifndef OCTOON_ANIMATION_COMPONENT_H_
#define OCTOON_ANIMATION_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/animation/animation.h>

namespace octoon
{
	class OCTOON_EXPORT AnimationComponent : public GameComponent
	{
		OctoonDeclareSubInterface(AnimationComponent, GameComponent)
	public:
		AnimationComponent() noexcept;
		virtual ~AnimationComponent() noexcept;

		virtual bool play(std::string_view status = "default") noexcept = 0;
		virtual void pause() noexcept = 0;
		virtual void reset() noexcept = 0;

		virtual void setTime(float time) noexcept = 0;
		virtual float getTime() const noexcept = 0;

		virtual void sample(float delta = 0.0f) noexcept = 0;
		virtual void evaluate(float delta = 0.0f) noexcept = 0;

		virtual const animation::AnimatorStateInfo<float>& getCurrentAnimatorStateInfo() const noexcept = 0;

	private:
		AnimationComponent(const AnimationComponent&) noexcept = delete;
		AnimationComponent& operator=(const AnimationComponent&) noexcept = delete;
	};
}

#endif