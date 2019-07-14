#include <octoon/animation_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/math/quat.h>

namespace octoon
{
	OctoonImplementSubClass(AnimationComponent, GameComponent, "Animation")

	AnimationComponent::AnimationComponent() noexcept
		: timer_(nullptr)
	{
	}

	AnimationComponent::AnimationComponent(animation::AnimationClip<float>&& clip) noexcept
		: timer_(nullptr)
	{
		clips_.emplace_back(clip);
	}

	AnimationComponent::AnimationComponent(animation::AnimationClips<float>&& clips) noexcept
		: timer_(nullptr)
		, clips_(std::move(clips))
	{
	}

	AnimationComponent::AnimationComponent(const animation::AnimationClip<float>& clip) noexcept
		: timer_(nullptr)
	{
		clips_.emplace_back(clip);
	}

	AnimationComponent::AnimationComponent(const animation::AnimationClips<float>& clips) noexcept
		: timer_(nullptr)
		, clips_(clips)
	{
	}

	AnimationComponent::~AnimationComponent() noexcept
	{
	}

	void
	AnimationComponent::play() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FrameEnd);
	}

	void
	AnimationComponent::pause() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
	}

	void
	AnimationComponent::stop() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
	}

	GameComponentPtr
	AnimationComponent::clone() const noexcept
	{
		return std::make_shared<AnimationComponent>();
	}

	void 
	AnimationComponent::onActivate() except
	{
		timer_ = this->getGameObject()->getGameScene()->getFeature<TimerFeature>();
	}

	void
	AnimationComponent::onDeactivate() noexcept
	{
		timer_ = nullptr;
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
	}

	void
	AnimationComponent::onFrameEnd() except
	{
		auto delta = timer_->delta();

		for (auto& clip : clips_)
		{
			for (auto& curve : clip.curves)
				curve.second.evaluate(delta);

			auto transform = this->getComponent<TransformComponent>();
			auto translate = transform->getLocalTranslate();
			auto quat = transform->getLocalQuaternion();
			auto eular = math::float3::Zero;

			for (auto& curve : clip.curves)
			{
				if (curve.first == "LocalPosition.x")
					translate.x = curve.second.key.value;
				else if (curve.first == "LocalPosition.y")
					translate.y = curve.second.key.value;
				else if (curve.first == "LocalPosition.z")
					translate.z = curve.second.key.value;
				else if (curve.first == "LocalRotation.x")
					eular.x = curve.second.key.value;
				else if (curve.first == "LocalRotation.y")
					eular.y = curve.second.key.value;
				else if (curve.first == "LocalRotation.z")
					eular.z = curve.second.key.value;
				else if (curve.first == "Transform:move")
					translate += math::rotate(quat, math::float3::Forward) * curve.second.key.value;
			}

			transform->setLocalTranslate(translate);
			transform->setLocalQuaternion(math::Quaternion(eular));
		}
	}
}