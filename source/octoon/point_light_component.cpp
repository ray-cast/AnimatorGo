#include <octoon/point_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(PointLightComponent, LightComponent, "PointLight")

	PointLightComponent::PointLightComponent() noexcept
		: shadowBias_(0.0f)
		, shadowEnable_(false)
		, shadowMapSize_(512, 512)
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

	void
	PointLightComponent::setRadius(float radius) noexcept
	{
		if (pointLight_)
			pointLight_->setRange(radius);
		shadowRadius_ = radius;
	}

	float
	PointLightComponent::getRadius() const noexcept
	{
		return shadowRadius_;
	}

	void
	PointLightComponent::setShadowEnable(bool enable) noexcept
	{
		if (this->pointLight_)
			this->pointLight_->setShadowEnable(enable);
		this->shadowEnable_ = enable;
	}
	
	bool
	PointLightComponent::getShadowEnable() const noexcept
	{
		return this->shadowEnable_;
	}

	void
	PointLightComponent::setShadowBias(float bias) noexcept
	{
		if (this->pointLight_)
			this->pointLight_->setShadowBias(bias);
		this->shadowBias_ = bias;
	}

	float
	PointLightComponent::getShadowBias() const noexcept
	{
		return this->shadowBias_;
	}

	void
	PointLightComponent::setShadowMapSize(const math::uint2& size) noexcept
	{
		if (this->pointLight_)
			this->pointLight_->setShadowMapSize(size);
		this->shadowMapSize_ = size;
	}

	const math::uint2&
	PointLightComponent::getShadowMapSize() const noexcept
	{
		return this->shadowMapSize_;
	}

	GameComponentPtr
	PointLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PointLightComponent>();
		instance->setName(this->getName());
		instance->setColor(this->getColor());
		instance->setIntensity(this->getIntensity());
		instance->setSize(this->getSize());
		instance->setRadius(this->getRadius());
		instance->setShadowBias(this->getShadowBias());
		instance->setShadowEnable(this->getShadowEnable());
		instance->setShadowMapSize(this->getShadowMapSize());

		return instance;
	}

	void
	PointLightComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();

		pointLight_ = std::make_shared<PointLight>();
		pointLight_->setLayer(this->getGameObject()->getLayer());
		pointLight_->setColor(this->getColor());
		pointLight_->setIntensity(this->getIntensity());
		pointLight_->setShadowBias(this->getShadowBias());
		pointLight_->setShadowEnable(this->getShadowEnable());
		pointLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->getFeature<VideoFeature>()->getMainScene()->addRenderObject(pointLight_.get());
		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	PointLightComponent::onDeactivate() noexcept
	{
		this->getFeature<VideoFeature>()->getMainScene()->removeRenderObject(pointLight_.get());
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
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