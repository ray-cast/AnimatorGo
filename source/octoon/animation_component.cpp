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

	AnimationComponent::AnimationComponent(animation::AnimationClip<float>&& clip) noexcept
		: AnimationComponent()
	{
		clips_.emplace_back(clip);
	}

	AnimationComponent::AnimationComponent(animation::AnimationClips<float>&& clips) noexcept
		: AnimationComponent()
	{
		clips_ = std::move(clips);
	}

	AnimationComponent::AnimationComponent(const animation::AnimationClip<float>& clip) noexcept
		: AnimationComponent()
	{
		clips_.emplace_back(clip);
	}

	AnimationComponent::AnimationComponent(const animation::AnimationClips<float>& clips) noexcept
		: AnimationComponent()
	{
		clips_ = clips;
	}

	AnimationComponent::~AnimationComponent() noexcept
	{
	}

	void
	AnimationComponent::play() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FrameBegin);
		this->update();
	}

	void
	AnimationComponent::pause() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
	}

	void
	AnimationComponent::stop() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
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
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
	}

	void
	AnimationComponent::onFixedUpdate() except
	{
		auto timeFeature = this->getGameScene()->getFeature<TimerFeature>();
		if (timeFeature)
			this->update(timeFeature->getTimeInterval() / CLOCKS_PER_SEC);
	}

	void
	AnimationComponent::update(float delta) noexcept
	{
		for (auto& clip : clips_)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto scale = transform->getLocalScale();
			auto quat = transform->getLocalQuaternion();
			auto translate = transform->getLocalTranslate();
			auto euler = math::eulerAngles(quat);

			clip.evaluate(delta);

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
					translate += math::rotate(math::Quaternion(euler), math::float3::Forward) * curve.second.key.value;
				else
					this->sendMessage(curve.first, curve.second.key.value);
			}

			transform->setLocalScale(scale);
			transform->setLocalTranslate(translate);
			transform->setLocalQuaternion(math::Quaternion(euler));
		}

		this->sendMessageDownwards("octoon:animation:update");
	}
}