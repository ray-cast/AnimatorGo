#include <octoon/animation_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(AnimationComponent, GameComponent, "Animation")

	AnimationComponent::AnimationComponent() noexcept
		: time_(0.0f)
		, timeStep_(1000.0f / 24.0f)
		, timeInterval_(1000.0f / 24.0f)
	{
	}

	AnimationComponent::AnimationComponent(animation::Animation<float>&& animation) noexcept
		: AnimationComponent()
	{
		animation_ = std::move(animation);
	}

	AnimationComponent::AnimationComponent(const animation::Animation<float>& animation) noexcept
		: AnimationComponent()
	{
		animation_ = animation;
	}

	AnimationComponent::~AnimationComponent() noexcept
	{
	}

	void
	AnimationComponent::play() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FixedUpdate);
		this->update();
	}

	void
	AnimationComponent::pause() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimationComponent::stop() noexcept
	{
		this->setTime(0.0f);
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimationComponent::setTime(float time) noexcept
	{
		animation_.setTime(time);
		this->update();
	}

	void
	AnimationComponent::setAnimation(animation::Animation<float>&& clips) noexcept
	{
		animation_ = std::move(clips);
	}

	void
	AnimationComponent::setAnimation(const animation::Animation<float>& clips) noexcept
	{
		animation_ = clips;
	}

	const animation::Animation<float>&
	AnimationComponent::getAnimation() const noexcept
	{
		return animation_;
	}

	GameComponentPtr
	AnimationComponent::clone() const noexcept
	{
		return std::make_shared<AnimationComponent>();
	}

	void 
	AnimationComponent::onActivate() except
	{
	}

	void
	AnimationComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimationComponent::onFixedUpdate() except
	{
		auto timeFeature = this->getGameScene()->getFeature<TimerFeature>();
		if (timeFeature)
			this->update(timeFeature->getTimeInterval());
	}

	void
	AnimationComponent::update(float delta) noexcept
	{
		animation_.evaluate(delta);

		for (auto& clip : animation_.clips)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto scale = transform->getLocalScale();
			auto quat = transform->getLocalQuaternion();
			auto translate = transform->getLocalTranslate();
			auto euler = math::eulerAngles(quat);
			auto move = 0.0f;

			for (auto& curve : clip.curves)
			{
				if (curve.first == "LocalScale.x")
					scale.x = curve.second.key.value;
				else if (curve.first == "LocalScale.y")
					scale.y = curve.second.key.value;
				else if (curve.first == "LocalScale.z")
					scale.z = curve.second.key.value;
				else if (curve.first == "LocalPosition.x")
					translate.x = curve.second.key.value;
				else if (curve.first == "LocalPosition.y")
					translate.y = curve.second.key.value;
				else if (curve.first == "LocalPosition.z")
					translate.z = curve.second.key.value;
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
				else if (curve.first == "Transform:move")
					move = curve.second.key.value;
				else
					this->sendMessage(curve.first, curve.second.key.value);
			}

			auto rotation = math::Quaternion(euler);

			transform->setLocalScale(scale);
			transform->setLocalTranslate(translate + math::rotate(rotation, math::float3::Forward) * move);
			transform->setLocalQuaternion(rotation);
		}

		this->sendMessage("octoon:animation:update");
	}
}