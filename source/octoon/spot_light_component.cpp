#include <octoon/spot_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(SpotLightComponent, LightComponent, "SpotLight")

	SpotLightComponent::SpotLightComponent() noexcept
	{
	}

	SpotLightComponent::~SpotLightComponent() noexcept
	{
	}
	
	void
	SpotLightComponent::setIntensity(float value) noexcept
	{
		if (spotLight_)
			spotLight_->setIntensity(value);
		LightComponent::setIntensity(value);
	}

	void
	SpotLightComponent::setColor(const math::float3& value) noexcept
	{
		if (spotLight_)
			spotLight_->setColor(value);
		LightComponent::setColor(value);
	}

	GameComponentPtr
	SpotLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SpotLightComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void
	SpotLightComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();

		spotLight_ = std::make_shared<light::SpotLight>();
		spotLight_->setActive(true);
		spotLight_->setLayer(this->getGameObject()->getLayer());
		spotLight_->setColor(this->getColor());
		spotLight_->setIntensity(this->getIntensity());
		spotLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	SpotLightComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		spotLight_->setActive(false);
	}

	void
	SpotLightComponent::onMoveAfter() noexcept
	{
		if (this->spotLight_)
		{
			auto transform = this->getComponent<TransformComponent>();
			this->spotLight_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	SpotLightComponent::onLayerChangeAfter() noexcept
	{
		if (this->spotLight_)
			this->spotLight_->setLayer(this->getGameObject()->getLayer());
	}
}