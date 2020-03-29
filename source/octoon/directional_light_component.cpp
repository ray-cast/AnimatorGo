#include <octoon/directional_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(DirectionalLightComponent, LightComponent, "DirectionalLight")

	DirectionalLightComponent::DirectionalLightComponent() noexcept
	{
	}

	DirectionalLightComponent::~DirectionalLightComponent() noexcept
	{
	}
	
	void
	DirectionalLightComponent::setIntensity(float value) noexcept
	{
		if (directionalLight_)
			directionalLight_->setLightIntensity(value);
		LightComponent::setIntensity(value);
	}

	void
	DirectionalLightComponent::setColor(const math::float3& value) noexcept
	{
		if (directionalLight_)
			directionalLight_->setLightColor(value);
		LightComponent::setColor(value);
	}

	GameComponentPtr
	DirectionalLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<DirectionalLightComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void
	DirectionalLightComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();

		directionalLight_ = std::make_shared<light::DirectionalLight>();
		directionalLight_->setActive(true);
		directionalLight_->setLayer(this->getGameObject()->getLayer());
		directionalLight_->setLightColor(this->getColor());
		directionalLight_->setLightIntensity(this->getIntensity());
		directionalLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	DirectionalLightComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		directionalLight_->setActive(false);
	}

	void
	DirectionalLightComponent::onMoveAfter() noexcept
	{
		if (this->directionalLight_)
		{
			auto transform = this->getComponent<TransformComponent>();
			this->directionalLight_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	DirectionalLightComponent::onLayerChangeAfter() noexcept
	{
		if (this->directionalLight_)
			this->directionalLight_->setLayer(this->getGameObject()->getLayer());
	}
}