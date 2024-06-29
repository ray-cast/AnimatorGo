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
		AnimatorComponent(std::shared_ptr<Animation>&& animation, GameObjects&& avatar) noexcept;
		AnimatorComponent(std::shared_ptr<Animation>&& animation, const GameObjects& avatar) noexcept;
		AnimatorComponent(const std::shared_ptr<Animation>& animation, GameObjects&& avatar) noexcept;
		AnimatorComponent(const std::shared_ptr<Animation>& animation, const GameObjects& avatar) noexcept;
		explicit AnimatorComponent(std::shared_ptr<Animation>&& animation) noexcept;
		explicit AnimatorComponent(const std::shared_ptr<Animation>& animation) noexcept;
		explicit AnimatorComponent(GameObjects&& avatar) noexcept;
		explicit AnimatorComponent(const GameObjects& avatar) noexcept;
		~AnimatorComponent() noexcept;

		bool play(std::string_view status = "default") noexcept override;
		void pause() noexcept override;
		void reset() noexcept override;

		void setTime(float time) noexcept override;
		float getTime() const noexcept override;

		void sample(float delta = 0.0f) noexcept override;
		void evaluate(float delta = 0.0f) noexcept override;

		void setAvatar(GameObjects&& avatar) noexcept;
		void setAvatar(const GameObjects& avatar) noexcept;
		const GameObjects& getAvatar() const noexcept;

		void setAnimation(std::shared_ptr<Animation>&& animation) noexcept;
		void setAnimation(const std::shared_ptr<Animation>& animation) noexcept;
		
		const std::shared_ptr<Animation>& getAnimation() const noexcept;
		const AnimatorStateInfo& getCurrentAnimatorStateInfo() const noexcept override;

		void load(const nlohmann::json& json) noexcept(false) override;
		void save(nlohmann::json& json) const noexcept(false) override;

		GameComponentPtr clone() const noexcept;

	private:
		void onActivate() noexcept(false);
		void onDeactivate() noexcept;

		void onFixedUpdate() noexcept;

	private:
		void updateAvatar(float delta = 0.0f) noexcept;
		void updateAnimation(float delta = 0.0f) noexcept;

	private:
		void updateBindpose(const GameObjects& avatar) noexcept;
		void updateBindmap() noexcept;

	private:
		bool enableAnimation_;
		bool enableAnimOnVisableOnly_;

		math::float3s bindpose_;

		std::shared_ptr<Animation> animation_;
		std::unordered_map<std::string, std::size_t> bindmap_;

		GameObjects avatar_;
		AnimatorStateInfo animatorStateInfo_;
	};
}

#endif