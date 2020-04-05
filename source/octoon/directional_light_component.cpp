#include <octoon/directional_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(DirectionalLightComponent, LightComponent, "DirectionalLight")

	DirectionalLightComponent::DirectionalLightComponent() noexcept
		: shadowBias_(-0.00002f)
		, shadowRadius_(1.0f)
		, shadowEnable_(false)
		, shadowMapSize_(512, 512)
	{
	}

	DirectionalLightComponent::~DirectionalLightComponent() noexcept
	{
	}
	
	void
	DirectionalLightComponent::setIntensity(float value) noexcept
	{
		if (directionalLight_)
			directionalLight_->setIntensity(value);
		LightComponent::setIntensity(value);
	}

	void
	DirectionalLightComponent::setColor(const math::float3& value) noexcept
	{
		if (directionalLight_)
			directionalLight_->setColor(value);
		LightComponent::setColor(value);
	}

	void
	DirectionalLightComponent::setShadowEnable(bool enable) noexcept
	{
		if (this->directionalLight_)
			this->directionalLight_->setShadowEnable(enable);
		this->shadowEnable_ = enable;
	}
	
	bool
	DirectionalLightComponent::getShadowEnable() const noexcept
	{
		return this->shadowEnable_;
	}

	void
	DirectionalLightComponent::setShadowBias(float bias) noexcept
	{
		if (this->directionalLight_)
			this->directionalLight_->setShadowBias(bias);
		this->shadowBias_ = bias;
	}

	float
	DirectionalLightComponent::getShadowBias() const noexcept
	{
		return this->shadowBias_;
	}

	void
	DirectionalLightComponent::setShadowRadius(float radius) noexcept
	{
		if (this->directionalLight_)
			this->directionalLight_->setShadowRadius(radius);
		this->shadowRadius_ = radius;
	}

	float
	DirectionalLightComponent::getShadowRadius() const noexcept
	{
		return this->shadowRadius_;
	}

	void
	DirectionalLightComponent::setShadowMapSize(const math::float2& size) noexcept
	{
		if (this->directionalLight_)
			this->directionalLight_->setShadowMapSize(size);
		this->shadowMapSize_ = size;
	}

	const math::float2&
	DirectionalLightComponent::getShadowMapSize() const noexcept
	{
		return this->shadowMapSize_;
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
		directionalLight_->setColor(this->getColor());
		directionalLight_->setIntensity(this->getIntensity());
		directionalLight_->setShadowBias(this->getShadowBias());
		directionalLight_->setShadowRadius(this->getShadowRadius());
		directionalLight_->setShadowMapSize(this->getShadowMapSize());
		directionalLight_->setShadowEnable(this->getShadowEnable());
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