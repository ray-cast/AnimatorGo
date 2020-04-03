#include <octoon/light/point_light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon::light
{
	OctoonImplementSubClass(PointLight, Light, "PointLight")

	PointLight::PointLight() noexcept
		: shadowEnable_(false)
		, shadowBias_(0.1f)
	{
		auto shadowCamera = std::make_shared<camera::PerspectiveCamera>();
		shadowCamera->setAperture(90.0f);
		shadowCamera->setNear(0.1f);
		shadowCamera->setSensorSize(math::float2::One);
		shadowCamera->setOwnerListener(this);
		shadowCamera->setRenderOrder(-std::numeric_limits<std::int32_t>::max());
	}

	PointLight::~PointLight() noexcept
	{
	}

	void
	PointLight::setShadowEnable(bool enable) noexcept
	{
		if (this->shadowEnable_ != enable)
		{
			if (this->shadowCamera_)
				this->shadowCamera_->setActive(enable);
			this->shadowEnable_ = enable;
		}
	}

	bool
	PointLight::getShadowEnable() const noexcept
	{
		return this->shadowEnable_;
	}

	const std::shared_ptr<camera::Camera>&
	PointLight::getCamera() const noexcept
	{
		return this->shadowCamera_;
	}

	void
	PointLight::setShadowBias(float bias) noexcept
	{
		shadowBias_ = bias;
	}

	float
	PointLight::getShadowBias() const noexcept
	{
		return shadowBias_;
	}

	std::shared_ptr<video::RenderObject>
	PointLight::clone() const noexcept
	{
		auto light = std::make_shared<PointLight>();

		return light;
	}
}