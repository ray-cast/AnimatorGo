#include <octoon/animator_component.h>
#include <octoon/transform_component.h>
#include <octoon/solver_component.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(AnimatorComponent, GameComponent, "Animator")

	AnimatorComponent::AnimatorComponent() noexcept
		: enableAnimation_(true)
		, enableAnimOnVisableOnly_(false)
	{
	}

	AnimatorComponent::AnimatorComponent(animation::Animation<float>&& animation) noexcept
		: AnimatorComponent()
	{
		animation_ = std::move(animation);
	}

	AnimatorComponent::AnimatorComponent(const animation::Animation<float>& animation) noexcept
		: AnimatorComponent()
	{
		animation_ = animation;
	}

	AnimatorComponent::AnimatorComponent(GameObjects&& transforms) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(std::move(transforms));
	}

	AnimatorComponent::AnimatorComponent(const GameObjects& transforms) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(transforms);
	}

	AnimatorComponent::~AnimatorComponent() noexcept
	{
	}

	bool
	AnimatorComponent::play(const std::string& status) noexcept
	{
		this->setName(status);
		this->addComponentDispatch(GameDispatchType::FixedUpdate);
		this->setTime(0.0f);

		enableAnimation_ = true;
		return enableAnimation_;
	}

	void
	AnimatorComponent::pause() noexcept
	{
		enableAnimation_ = false;
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimatorComponent::stop() noexcept
	{
		this->setTime(0.0f);
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimatorComponent::setTime(float time) noexcept
	{
		animation_.setTime(time);
		this->update();
	}

	void
	AnimatorComponent::setAnimation(animation::Animation<float>&& clips) noexcept
	{
		animation_ = std::move(clips);
	}

	void
	AnimatorComponent::setAnimation(const animation::Animation<float>& clips) noexcept
	{
		animation_ = clips;
	}

	const animation::Animation<float>&
	AnimatorComponent::getAnimation() const noexcept
	{
		return animation_;
	}

	void
	AnimatorComponent::setAvatar(GameObjects&& avatar) noexcept
	{
		avatar_ = std::move(avatar);
		this->updateBindpose(avatar_);
	}

	void
	AnimatorComponent::setAvatar(const GameObjects& avatar) noexcept
	{
		avatar_ = avatar;
		this->updateBindpose(avatar_);
	}

	const GameObjects&
	AnimatorComponent::getAvatar() const noexcept
	{
		return avatar_;
	}

	GameComponentPtr
	AnimatorComponent::clone() const noexcept
	{
		auto instance = std::make_shared<AnimatorComponent>();
		instance->setName(this->getName());
		instance->setAvatar(this->getAvatar());
		instance->setAnimation(this->getAnimation());

		return instance;
	}

	void 
	AnimatorComponent::onActivate() except
	{
	}

	void
	AnimatorComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimatorComponent::onFixedUpdate() noexcept
	{
		if (enableAnimation_)
		{
			auto timeFeature = this->getFeature<TimerFeature>();
			if (timeFeature)
				this->update(timeFeature->getTimeInterval());
		}
	}

	void
	AnimatorComponent::updateBindpose(const GameObjects& bones) noexcept
	{
		bindpose_.resize(bones.size());

		for (std::size_t i = 0; i < bones.size(); i++)
			bindpose_[i] = bones[i]->getComponent<TransformComponent>()->getLocalTranslate();
	}

	void
	AnimatorComponent::update(float delta) noexcept
	{
		animation_.evaluate(delta);

		for (std::size_t i = 0; i < animation_.clips.size(); i++)
		{
			//assert(avatar_[i]->getName() == clips_[i].name);
			if (animation_.clips[i].finish)
				continue;

			auto transform = avatar_[i]->getComponent<TransformComponent>();

			auto scale = transform->getLocalScale();
			auto quat = transform->getLocalQuaternion();
			auto translate = transform->getLocalTranslate();
			auto euler = math::eulerAngles(quat);

			for (auto& curve : animation_.clips[i].curves)
			{
				if (curve.first == "LocalPosition.x")
					translate.x = curve.second.key.value + bindpose_[i].x;
				else if (curve.first == "LocalPosition.y")
					translate.y = curve.second.key.value + bindpose_[i].y;
				else if (curve.first == "LocalPosition.z")
					translate.z = curve.second.key.value + bindpose_[i].z;
				else if (curve.first == "LocalScale.x")
					scale.x = curve.second.key.value;
				else if (curve.first == "LocalScale.y")
					scale.y = curve.second.key.value;
				else if (curve.first == "LocalScale.z")
					scale.z = curve.second.key.value;
				else if (curve.first == "LocalRotation.x")
					quat.x = curve.second.key.value;
				else if (curve.first == "LocalRotation.y")
					quat.y = curve.second.key.value;
				else if (curve.first == "LocalRotation.z")
					quat.z = curve.second.key.value;
				else if (curve.first == "LocalRotation.w")
					quat.w = curve.second.key.value;
				else if (curve.first == "LocalEulerAnglesRaw.x")
					euler.x = curve.second.key.value;
				else if (curve.first == "LocalEulerAnglesRaw.y")
					euler.y = curve.second.key.value;
				else if (curve.first == "LocalEulerAnglesRaw.z")
					euler.z = curve.second.key.value;
			}

			transform->setLocalScale(scale);
			transform->setLocalTranslate(translate);
			transform->setLocalQuaternion(math::Quaternion(euler));
		}

		this->sendMessage("octoon:animation:update");
	}
}