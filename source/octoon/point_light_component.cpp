#include <octoon/point_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(PointLightComponent, LightComponent, "PointLight")

	PointLightComponent::PointLightComponent() noexcept
	{
	}

	PointLightComponent::~PointLightComponent() noexcept
	{
	}
	
	void
	PointLightComponent::setIntensity(float value) noexcept
	{
		if (pointLight_)
			pointLight_->setIntensity(value);
		LightComponent::setIntensity(value);
	}

	void
	PointLightComponent::setColor(const math::float3& value) noexcept
	{
		if (pointLight_)
			pointLight_->setColor(value);
		LightComponent::setColor(value);
	}

	GameComponentPtr
	PointLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PointLightComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void
	PointLightComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();

		pointLight_ = std::make_shared<light::PointLight>();
		pointLight_->setActive(true);
		pointLight_->setLayer(this->getGameObject()->getLayer());
		pointLight_->setColor(this->getColor());
		pointLight_->setIntensity(this->getIntensity());
		pointLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	PointLightComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		pointLight_->setActive(false);
	}

	void
	PointLightComponent::onMoveAfter() noexcept
	{
		if (this->pointLight_)
		{
			auto transform = this->getComponent<TransformComponent>();
			this->pointLight_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	PointLightComponent::onLayerChangeAfter() noexcept
	{
		if (this->pointLight_)
			this->pointLight_->setLayer(this->getGameObject()->getLayer());
	}
}