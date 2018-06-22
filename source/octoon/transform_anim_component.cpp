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
		scale_ = std::move(frames);
	}

	void
	TransformAnimComponent::setRotation(model::Keyframes<math::float3>&& frames) noexcept
	{
		rotation_ = std::move(frames);
	}

	void
	TransformAnimComponent::setTranslate(model::Keyframes<math::float3>&& frames) noexcept
	{
		pos_ = std::move(frames);
	}

	void
	TransformAnimComponent::setScale(const model::Keyframes<math::float3>& frames) noexcept
	{
		scale_ = frames;
	}

	void
	TransformAnimComponent::setRotation(const model::Keyframes<math::float3>& frames) noexcept
	{
		rotation_ = frames;
	}

	void
	TransformAnimComponent::setTranslate(const model::Keyframes<math::float3>& frames) noexcept
	{
		pos_ = frames;
	}

	GameComponentPtr
	TransformAnimComponent::clone() const noexcept
	{
		auto instance = std::make_shared<TransformAnimComponent>();
		instance->setName(instance->getName());
		instance->pos_ = this->pos_;
		instance->scale_ = this->scale_;
		instance->rotation_ = this->rotation_;

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
			float step = 0;
#if OCTOON_FEATURE_TIMER_ENABLE
			step = GameApp::instance()->getFeature<TimerFeature>()->delta();
#endif
			if (!pos_.frames.empty())
				transform->setTranslate(pos_.evaluate(step));

			if (!scale_.frames.empty())
				transform->setScale(scale_.evaluate(step));

			if (!rotation_.frames.empty())
				transform->setQuaternion(math::Quaternion(rotation_.evaluate(step)));
		}

	}
}