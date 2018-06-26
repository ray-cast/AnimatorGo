#include <octoon/transform_anim_component.h>
#include <octoon/game_app.h>
#include <octoon/transform_component.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(TransformAnimComponent, GameComponent, "MeshFilter")

	TransformAnimComponent::TransformAnimComponent() noexcept
	{
	}

	TransformAnimComponent::~TransformAnimComponent() noexcept
	{
	}

	void
	TransformAnimComponent::setScale(model::Keyframes<math::float3>&& frames) noexcept
	{
		scale_.frames = std::move(frames);
	}

	void
	TransformAnimComponent::setRotation(model::Keyframes<math::float3>&& frames) noexcept
	{
		rotation_.frames = std::move(frames);
	}

	void
	TransformAnimComponent::setTranslate(model::Keyframes<math::float3>&& frames) noexcept
	{
		pos_.frames = std::move(frames);
	}

	void
	TransformAnimComponent::setScale(const model::Keyframes<math::float3>& frames) noexcept
	{
		scale_.frames = frames;
	}

	void
	TransformAnimComponent::setRotation(const model::Keyframes<math::float3>& frames) noexcept
	{
		rotation_.frames = frames;
	}

	void
	TransformAnimComponent::setTranslate(const model::Keyframes<math::float3>& frames) noexcept
	{
		pos_.frames = frames;
	}

	void
	TransformAnimComponent::setLocalScale(model::Keyframes<math::float3>&& frames) noexcept
	{
		localScale_.frames = std::move(frames);
	}

	void
	TransformAnimComponent::setLocalRotation(model::Keyframes<math::float3>&& frames) noexcept
	{
		localRotation_.frames = std::move(frames);
	}

	void
	TransformAnimComponent::setLocalTranslate(model::Keyframes<math::float3>&& frames) noexcept
	{
		localPos_.frames = std::move(frames);
	}

	void
	TransformAnimComponent::setLocalScale(const model::Keyframes<math::float3>& frames) noexcept
	{
		localScale_.frames = frames;
	}

	void
	TransformAnimComponent::setLocalRotation(const model::Keyframes<math::float3>& frames) noexcept
	{
		localRotation_.frames = frames;
	}

	void
	TransformAnimComponent::setLocalTranslate(const model::Keyframes<math::float3>& frames) noexcept
	{
		localPos_.frames = frames;
	}

	void
	TransformAnimComponent::setScale(model::AnimationCurve<math::float3>&& frames) noexcept
	{
		scale_ = std::move(frames);
	}

	void
	TransformAnimComponent::setRotation(model::AnimationCurve<math::float3>&& frames) noexcept
	{
		rotation_ = std::move(frames);
	}

	void
	TransformAnimComponent::setTranslate(model::AnimationCurve<math::float3>&& frames) noexcept
	{
		pos_ = std::move(frames);
	}

	void
	TransformAnimComponent::setScale(const model::AnimationCurve<math::float3>& frames) noexcept
	{
		scale_ = frames;
	}

	void
	TransformAnimComponent::setRotation(const model::AnimationCurve<math::float3>& frames) noexcept
	{
		rotation_ = frames;
	}

	void
	TransformAnimComponent::setTranslate(const model::AnimationCurve<math::float3>& frames) noexcept
	{
		pos_ = frames;
	}

	void
	TransformAnimComponent::setLocalScale(model::AnimationCurve<math::float3>&& frames) noexcept
	{
		localScale_ = std::move(frames);
	}

	void
	TransformAnimComponent::setLocalRotation(model::AnimationCurve<math::float3>&& frames) noexcept
	{
		localRotation_ = std::move(frames);
	}

	void
	TransformAnimComponent::setLocalTranslate(model::AnimationCurve<math::float3>&& frames) noexcept
	{
		localPos_ = std::move(frames);
	}

	void
	TransformAnimComponent::setLocalScale(const model::AnimationCurve<math::float3>& frames) noexcept
	{
		localScale_ = frames;
	}

	void
	TransformAnimComponent::setLocalRotation(const model::AnimationCurve<math::float3>& frames) noexcept
	{
		localRotation_ = frames;
	}

	void
	TransformAnimComponent::setLocalTranslate(const model::AnimationCurve<math::float3>& frames) noexcept
	{
		localPos_ = frames;
	}

	GameComponentPtr
	TransformAnimComponent::clone() const noexcept
	{
		auto instance = std::make_shared<TransformAnimComponent>();
		instance->setName(instance->getName());
		instance->setScale(this->pos_);
		instance->setRotation(this->scale_);
		instance->setTranslate(this->rotation_);
		instance->setLocalScale(this->localPos_);
		instance->setLocalRotation(this->localScale_);
		instance->setLocalTranslate(this->localRotation_);

		return instance;
	}

	void
	TransformAnimComponent::onActivate()
	{
		this->addComponentDispatch(octoon::GameDispatchType::Frame);
	}

	void
	TransformAnimComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatchs();
	}

	void 
	TransformAnimComponent::onFrame() except
	{
		auto transform = this->getGameObject()->getComponent<TransformComponent>();
		if (transform)
		{
			float step = 1.0f / 60.0f;
#if OCTOON_FEATURE_TIMER_ENABLE
			// step = GameApp::instance()->getFeature<TimerFeature>()->delta();
#endif
			if (!pos_.empty())
				transform->setTranslate(pos_.evaluate(step));

			if (!scale_.empty())
				transform->setScale(scale_.evaluate(step));

			if (!rotation_.empty())
				transform->setQuaternion(math::Quaternion(rotation_.evaluate(step)));

			if (!localPos_.empty())
				transform->setLocalTranslate(localPos_.evaluate(step));

			if (!localScale_.empty())
				transform->setLocalScale(localScale_.evaluate(step));

			if (!localRotation_.empty())
				transform->setLocalQuaternion(math::Quaternion(localRotation_.evaluate(step)));
		}
	}
}