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
	EnvironmentLightComponent::setEnvironmentMap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		if (environmentLight_)
			environmentLight_->setEnvironmentMap(texture);
		environmentMap_ = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLightComponent::getEnvironmentMap() const noexcept
	{
		return environmentMap_;
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
		environmentLight_->setLayer(this->getGameObject()->getLayer());
		environmentLight_->setColor(this->getColor());
		environmentLight_->setIntensity(this->getIntensity());
		environmentLight_->setEnvironmentMap(this->environmentMap_);
		environmentLight_->setTransform(transform->getTransform(), transform->getTransformInverse());

		this->getFeature<VideoFeature>()->getMainScene()->addRenderObject(environmentLight_.get());
		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	EnvironmentLightComponent::onDeactivate() noexcept
	{
		this->getFeature<VideoFeature>()->getMainScene()->removeRenderObject(environmentLight_.get());
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
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