#include <octoon/light/directional_light.h>
#include <octoon/camera/ortho_camera.h>

namespace octoon
{
	OctoonImplementSubClass(DirectionalLight, Light, "DirectionalLight")

	DirectionalLight::DirectionalLight() noexcept
		: shadowBias_(0.0f)
		, shadowRadius_(1.0f)
		, shadowEnable_(false)
		, shadowSize_(512, 512)
	{
		this->shadowCamera_ = std::make_shared<OrthographicCamera>(-20.0f, 20.0f, -20.0f, 20.0f, 0.01f, 1000.f);
		this->shadowCamera_->setOwnerListener(this);
	}

	DirectionalLight::~DirectionalLight() noexcept
	{
	}

	void
	DirectionalLight::setShadowEnable(bool enable) noexcept
	{
		if (this->shadowEnable_ != enable)
		{
			if (this->shadowCamera_ && enable)
				this->shadowCamera_->setupFramebuffers(shadowSize_.x, shadowSize_.y, 0, GraphicsFormat::R8G8B8A8UNorm, GraphicsFormat::D32_SFLOAT);
			this->setDirty(true);
			this->shadowEnable_ = enable;
		}
	}

	bool
	DirectionalLight::getShadowEnable() const noexcept
	{
		return this->shadowEnable_;
	}

	void
	DirectionalLight::setShadowBias(float bias) noexcept
	{
		this->setDirty(true);
		shadowBias_ = bias;
	}

	void
	DirectionalLight::setShadowRadius(float radius) noexcept
	{
		this->setDirty(true);
		shadowRadius_ = radius;
	}

	float
	DirectionalLight::getShadowBias() const noexcept
	{
		return shadowBias_;
	}

	float
	DirectionalLight::getShadowRadius() const noexcept
	{
		return shadowRadius_;
	}

	void
	DirectionalLight::setShadowMapSize(const math::uint2& size) noexcept
	{
		if (this->shadowSize_ != size)
		{
			if (this->shadowCamera_ && this->shadowEnable_)
				this->shadowCamera_->setupFramebuffers(size.x, size.y, 0, GraphicsFormat::R8G8B8A8UNorm, GraphicsFormat::D32_SFLOAT);
			this->shadowSize_ = size;
		}
	}

	const math::uint2&
	DirectionalLight::getShadowMapSize() const noexcept
	{
		return this->shadowSize_;
	}

	void
	DirectionalLight::setCamera(const std::shared_ptr<Camera>& camera) noexcept
	{
		this->setDirty(true);
		this->shadowCamera_ = camera;
	}

	const std::shared_ptr<Camera>&
	DirectionalLight::getCamera() const noexcept
	{
		return this->shadowCamera_;
	}

	std::shared_ptr<RenderObject>
	DirectionalLight::clone() const noexcept
	{
		auto light = std::make_shared<DirectionalLight>();
		light->setShadowBias(this->getShadowBias());
		light->setShadowRadius(this->getShadowRadius());
		return light;
	}

	void
	DirectionalLight::onMoveAfter() noexcept
	{
		if (this->shadowCamera_)
			this->shadowCamera_->setTransform(this->getTransform(), this->getTransformInverse());
		Light::onMoveAfter();
	}
}