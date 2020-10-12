#include <octoon/environment_light_component.h>
#include <octoon/video_feature.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(EnvironmentLightComponent, LightComponent, "EnvironmentLight")

	EnvironmentLightComponent::EnvironmentLightComponent() noexcept
		: offset_(0, 0)
		, showBackground_(true)
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
	EnvironmentLightComponent::setOffset(const math::float2& value) noexcept
	{
		this->offset_ = value;
		if (environmentLight_)
			environmentLight_->setOffset(value);
	}

	const math::float2&
	EnvironmentLightComponent::getOffset() const noexcept
	{
		return this->offset_;
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

	void
	EnvironmentLightComponent::setShowBackground(bool show) noexcept
	{
		if (environmentLight_)
			environmentLight_->setShowBackground(show);
		this->showBackground_ = show;
	}

	bool
	EnvironmentLightComponent::getShowBackground() const noexcept
	{
		return this->showBackground_;
	}

	void
	EnvironmentLightComponent::setBackgroundMap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		if (environmentLight_)
			environmentLight_->setBackgroundMap(texture);
		this->backgroundMap_ = texture;
		
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLightComponent::getBackgroundMap() const noexcept
	{
		return this->backgroundMap_;
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
		environmentLight_->setShowBackground(this->showBackground_);
		environmentLight_->setBackgroundMap(this->backgroundMap_);
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