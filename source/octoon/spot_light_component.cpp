#include <octoon/spot_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(SpotLightComponent, LightComponent, "SpotLight")

	SpotLightComponent::SpotLightComponent() noexcept
		: shadowBias_(0.0f)
		, shadowEnable_(false)
		, shadowMapSize_(512, 512)
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

	void
	SpotLightComponent::setShadowEnable(bool enable) noexcept
	{
		if (this->spotLight_)
			this->spotLight_->setShadowEnable(enable);
		this->shadowEnable_ = enable;
	}
	
	bool
	SpotLightComponent::getShadowEnable() const noexcept
	{
		return this->shadowEnable_;
	}

	void
	SpotLightComponent::setShadowBias(float bias) noexcept
	{
		if (this->spotLight_)
			this->spotLight_->setShadowBias(bias);
		this->shadowBias_ = bias;
	}

	float
	SpotLightComponent::getShadowBias() const noexcept
	{
		return this->shadowBias_;
	}

	void
	SpotLightComponent::setShadowMapSize(const math::uint2& size) noexcept
	{
		if (this->spotLight_)
			this->spotLight_->setShadowMapSize(size);
		this->shadowMapSize_ = size;
	}

	const math::uint2&
	SpotLightComponent::getShadowMapSize() const noexcept
	{
		return this->shadowMapSize_;
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

		spotLight_ = std::make_shared<SpotLight>();
		spotLight_->setLayer(this->getGameObject()->getLayer());
		spotLight_->setColor(this->getColor());
		spotLight_->setIntensity(this->getIntensity());
		spotLight_->setShadowBias(this->getShadowBias());
		spotLight_->setShadowEnable(this->getShadowEnable());
		spotLight_->setShadowMapSize(this->getShadowMapSize());
		spotLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->getFeature<VideoFeature>()->getMainScene()->addRenderObject(spotLight_.get());
		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	SpotLightComponent::onDeactivate() noexcept
	{
		this->getFeature<VideoFeature>()->getMainScene()->removeRenderObject(spotLight_.get());
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
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