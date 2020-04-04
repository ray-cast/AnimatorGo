#include <octoon/light/directional_light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon::light
{
	OctoonImplementSubClass(DirectionalLight, Light, "DirectionalLight")

	DirectionalLight::DirectionalLight() noexcept
		: shadowBias_(0.0f)
		, shadowRadius_(1.0f)
		, shadowEnable_(false)
	{
		this->shadowCamera_ = std::make_shared<camera::PerspectiveCamera>(45.f, 0.1f, 1000.f);
		this->shadowCamera_->setOwnerListener(this);
		this->shadowCamera_->setRenderOrder(-std::numeric_limits<std::int32_t>::max());
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
			{
				if (enable)
				{
					this->shadowCamera_->setActive(enable);
					this->shadowCamera_->setupFramebuffers(512, 512, 0, hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat::D32_SFLOAT);
				}
				else
				{
					this->shadowCamera_->setActive(false);
				}
			}

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
		if (this->shadowCamera_)
			this->shadowCamera_->setActive(true);
		Light::onActivate();
	}

	void
	DirectionalLight::onDeactivate() noexcept
	{
		if (this->shadowCamera_)
			this->shadowCamera_->setActive(false);
		Light::onDeactivate();
	}

	void
	DirectionalLight::onMoveAfter() noexcept
	{
		if (this->shadowCamera_)
			this->shadowCamera_->setTransform(this->getTransform(), this->getTransformInverse());
		Light::onMoveAfter();
	}
}