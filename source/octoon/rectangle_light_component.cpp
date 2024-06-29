#include <octoon/rectangle_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(RectangleLightComponent, LightComponent, "RectangleLight")

	RectangleLightComponent::RectangleLightComponent() noexcept
	{
	}

	RectangleLightComponent::~RectangleLightComponent() noexcept
	{
	}
	
	void
	RectangleLightComponent::setIntensity(float value) noexcept
	{
		if (rectangleLight_)
			rectangleLight_->setIntensity(value);
		LightComponent::setIntensity(value);
	}

	void
	RectangleLightComponent::setColor(const math::float3& value) noexcept
	{
		if (rectangleLight_)
			rectangleLight_->setColor(value);
		LightComponent::setColor(value);
	}

	GameComponentPtr
	RectangleLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RectangleLightComponent>();
		instance->setName(this->getName());
		instance->setSize(this->getSize());
		instance->setColor(this->getColor());
		instance->setIntensity(this->getIntensity());

		return instance;
	}

	void
	RectangleLightComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();

		rectangleLight_ = std::make_shared<RectangleLight>();
		rectangleLight_->setLayer(this->getGameObject()->getLayer());
		rectangleLight_->setColor(this->getColor());
		rectangleLight_->setIntensity(this->getIntensity());
		rectangleLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->getFeature<VideoFeature>()->getMainScene()->addRenderObject(rectangleLight_.get());
		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	RectangleLightComponent::onDeactivate() noexcept
	{
		this->getFeature<VideoFeature>()->getMainScene()->removeRenderObject(rectangleLight_.get());
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	RectangleLightComponent::onMoveAfter() noexcept
	{
		if (this->rectangleLight_)
		{
			auto transform = this->getComponent<TransformComponent>();
			this->rectangleLight_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	RectangleLightComponent::onLayerChangeAfter() noexcept
	{
		if (this->rectangleLight_)
			this->rectangleLight_->setLayer(this->getGameObject()->getLayer());
	}
}