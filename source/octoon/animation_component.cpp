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
		this->addComponentDispatch(GameDispatchType::FrameBegin);
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
		timer_ = this->getGameObject()->getGameScene()->getFeature<TimerFeature>();
	}

	void
	AnimationComponent::onDeactivate() noexcept
	{
		timer_ = nullptr;
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
	}

	void
	AnimationComponent::onFrameBegin() except
	{
		auto delta = timer_->delta();

		for (auto& clip : clips_)
		{
			for (auto& curve : clip.curves)
				curve.second.evaluate(delta);

			auto transform = this->getComponent<TransformComponent>();

			for (auto& curve : clip.curves)
			{
				if (curve.first == "LocalPosition.x")
				{
					auto translate = transform->getTranslate();
					translate.x = curve.second.key.value;
					transform->setTranslate(translate);
				}
				else if (curve.first == "LocalPosition.y")
				{
					auto translate = transform->getTranslate();
					translate.y = curve.second.key.value;
					transform->setTranslate(translate);
				}
				else if (curve.first == "LocalPosition.z")
				{
					auto translate = transform->getTranslate();
					translate.z = curve.second.key.value;
					transform->setTranslate(translate);
				}
				else if (curve.first == "LocalRotation.x")
				{
					auto eular = math::eulerAngles(transform->getQuaternion());
					eular.x = curve.second.key.value;
					transform->setQuaternion(math::Quaternion(eular));
				}
				else if (curve.first == "LocalRotation.y")
				{
					auto eular = math::eulerAngles(transform->getQuaternion());
					eular.y = curve.second.key.value;
					transform->setQuaternion(math::Quaternion(eular));
				}
				else if (curve.first == "LocalRotation.z")
				{
					auto eular = math::eulerAngles(transform->getQuaternion());
					eular.z = curve.second.key.value;
					transform->setQuaternion(math::Quaternion(eular));
				}
				else if (curve.first == "Transform:move")
				{
					auto translate = transform->getTranslate();
					translate += math::rotate(transform->getQuaternion(), math::float3::Forward) * curve.second.key.value;
					transform->setTranslate(translate);
				}
			}
		}
	}

	void
	AnimationComponent::onFrame() except
	{
	}

	void
	AnimationComponent::onFrameEnd() except
	{
	}
}