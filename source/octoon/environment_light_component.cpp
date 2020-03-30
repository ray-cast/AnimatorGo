#include <octoon/environment_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(EnvironmentLightComponent, LightComponent, "EnvironmentLight")

	EnvironmentLightComponent::EnvironmentLightComponent() noexcept
	{
	}

	EnvironmentLightComponent::~EnvironmentLightComponent() noexcept
	{
	}
	
	void
	EnvironmentLightComponent::setIntensity(float value) noexcept
	{
		if (environmentLight_)
			environmentLight_->setIntensity(value);
		LightComponent::setIntensity(value);
	}

	void
	EnvironmentLightComponent::setColor(const math::float3& value) noexcept
	{
		if (environmentLight_)
			environmentLight_->setColor(value);
		LightComponent::setColor(value);
	}

	void
	EnvironmentLightComponent::setIrradiance(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		if (environmentLight_)
			environmentLight_->setIrradiance(texture);
		irradiance_ = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLightComponent::getIrradiance() const noexcept
	{
		return irradiance_;
	}

	void
	EnvironmentLightComponent::setRadiance(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		if (environmentLight_)
			environmentLight_->setRadiance(texture);
		radiance_ = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLightComponent::getRadiance() const noexcept
	{
		return radiance_;
	}

	GameComponentPtr
	EnvironmentLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<EnvironmentLightComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void
	EnvironmentLightComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();

		environmentLight_ = std::make_shared<light::EnvironmentLight>();
		environmentLight_->setActive(true);
		environmentLight_->setLayer(this->getGameObject()->getLayer());
		environmentLight_->setColor(this->getColor());
		environmentLight_->setIntensity(this->getIntensity());
		environmentLight_->setIrradiance(this->irradiance_);
		environmentLight_->setRadiance(this->radiance_);
		environmentLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	EnvironmentLightComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		environmentLight_->setActive(false);
	}

	void
	EnvironmentLightComponent::onMoveAfter() noexcept
	{
		if (this->environmentLight_)
		{
			auto transform = this->getComponent<TransformComponent>();
			this->environmentLight_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	EnvironmentLightComponent::onLayerChangeAfter() noexcept
	{
		if (this->environmentLight_)
			this->environmentLight_->setLayer(this->getGameObject()->getLayer());
	}
}