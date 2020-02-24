#ifndef OCTOON_ANIMATOR_COMPONENT_H_
#define OCTOON_ANIMATOR_COMPONENT_H_

#include <octoon/animation_component.h>

namespace octoon
{
	class OCTOON_EXPORT AnimatorComponent final : public AnimationComponent
	{
		OctoonDeclareSubClass(AnimatorComponent, AnimationComponent)
	public:
		AnimatorComponent() noexcept;
		AnimatorComponent(animation::Animation<float>&& animation, GameObjects&& avatar) noexcept;
		AnimatorComponent(animation::Animation<float>&& animation, const GameObjects& avatar) noexcept;
		AnimatorComponent(const animation::Animation<float>& animation, GameObjects&& avatar) noexcept;
		AnimatorComponent(const animation::Animation<float>& animation, const GameObjects& avatar) noexcept;
		explicit AnimatorComponent(animation::Animation<float>&& animation) noexcept;
		explicit AnimatorComponent(const animation::Animation<float>& animation) noexcept;
		explicit AnimatorComponent(GameObjects&& avatar) noexcept;
		explicit AnimatorComponent(const GameObjects& avatar) noexcept;
		~AnimatorComponent() noexcept;

		bool play(const std::string& status = "default") noexcept override;
		void pause() noexcept override;
		void reset() noexcept override;

		void setTime(float time) noexcept override;
		float getTime() const noexcept override;

		void sample(float delta = 0.0f) noexcept override;
		void evaluate(float delta = 0.0f) noexcept override;

		void setAvatar(GameObjects&& avatar) noexcept;
		void setAvatar(const GameObjects& avatar) noexcept;
		const GameObjects& getAvatar() const noexcept;

		void setAnimation(animation::Animation<float>&& animation) noexcept;
		void setAnimation(const animation::Animation<float>& animation) noexcept;
		
		const animation::Animation<float>& getAnimation() const noexcept;
		const animation::AnimatorStateInfo<float>& getCurrentAnimatorStateInfo() const noexcept override;

		GameComponentPtr clone() const noexcept;

	private:
		void onActivate() except;
		void onDeactivate() noexcept;

		void onFixedUpdate() noexcept;

	private:
		void updateAvatar(float delta = 0.0f) noexcept;
		void updateAnimation(float delta = 0.0f) noexcept;

	private:
		void onAttachAvatar(const GameObjects& avatar) noexcept;

	private:
		bool enableAnimation_;
		bool enableAnimOnVisableOnly_;

		animation::Animation<float> animation_;
		math::float3s bindpose_;

		GameObjects avatar_;
	};
}

#endif