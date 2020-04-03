#include <octoon/light/directional_light.h>
#include <octoon/camera/ortho_camera.h>

namespace octoon::light
{
	OctoonImplementSubClass(DirectionalLight, Light, "DirectionalLight")

	DirectionalLight::DirectionalLight() noexcept
		: shadowBias_(0.0f)
		, shadowRadius_(0.0f)
		, shadowEnable_(false)
	{
		auto shadowCamera = std::make_shared<camera::OrthoCamera>(-100, 100, -100, 100, 0.1f, 100.f);
		shadowCamera->setOwnerListener(this);
		shadowCamera->setRenderOrder(-std::numeric_limits<std::int32_t>::max());
	}

	DirectionalLight::~DirectionalLight() noexcept
	{
	}

	void
	DirectionalLight::setShadowEnable(bool enable) noexcept
	{
		if (this->shadowEnable_ != enable)
		{
			if (this->shadowCamera_)
				this->shadowCamera_->setActive(enable);
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
		shadowBias_ = bias;
	}

	void
	DirectionalLight::setShadowRadius(float radius) noexcept
	{
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
	DirectionalLight::setCamera(const std::shared_ptr<camera::Camera>& camera) noexcept
	{
		this->shadowCamera_ = camera;
	}

	const std::shared_ptr<camera::Camera>&
	DirectionalLight::getCamera() const noexcept
	{
		return this->shadowCamera_;
	}

	std::shared_ptr<video::RenderObject>
	DirectionalLight::clone() const noexcept
	{
		auto light = std::make_shared<DirectionalLight>();
		light->setShadowBias(this->getShadowBias());
		light->setShadowRadius(this->getShadowRadius());
		return light;
	}

	void
	DirectionalLight::onActivate() noexcept
	{
		if (this->shadowCamera_ && shadowEnable_)
			this->shadowCamera_->setActive(true);
		Light::onActivate();
	}

	void
	DirectionalLight::onDeactivate() noexcept
	{
		if (this->shadowCamera_ && !shadowEnable_)
			this->shadowCamera_->setActive(false);
		Light::onDeactivate();
	}
}