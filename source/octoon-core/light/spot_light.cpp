#include <octoon/light/spot_light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon::light
{
	OctoonImplementSubClass(SpotLight, Light, "SpotLight")

	SpotLight::SpotLight() noexcept
		: _spotInnerCone(5.0f, math::cos(math::radians(5.0f)))
		, _spotOuterCone(40.0f, math::cos(math::radians(40.0f)))
		, shadowEnable_(false)
		, shadowBias_(0.1f)
	{
		auto shadowCamera = std::make_shared<camera::PerspectiveCamera>();
		shadowCamera->setAperture(90.0f);
		shadowCamera->setNear(0.1f);
		shadowCamera->setSensorSize(math::float2::One);
		shadowCamera->setOwnerListener(this);
		shadowCamera->setRenderOrder(-std::numeric_limits<std::int32_t>::max());
	}

	SpotLight::~SpotLight() noexcept
	{
	}

	void
	SpotLight::setInnerCone(float value) noexcept
	{
		_spotInnerCone.x = math::min(_spotOuterCone.x, value);
		_spotInnerCone.y = math::cos(math::radians(_spotInnerCone.x));
	}

	void
	SpotLight::setOuterCone(float value) noexcept
	{
		_spotOuterCone.x = math::max(_spotInnerCone.x, value);
		_spotOuterCone.y = math::cos(math::radians(_spotOuterCone.x));
	}

	const math::float2&
	SpotLight::getInnerCone() const noexcept
	{
		return _spotInnerCone;
	}

	const math::float2&
	SpotLight::getOuterCone() const noexcept
	{
		return _spotOuterCone;
	}

	void
	SpotLight::setShadowEnable(bool enable) noexcept
	{
		if (this->shadowEnable_ != enable)
		{
			if (this->shadowCamera_)
				this->shadowCamera_->setActive(enable);
			this->shadowEnable_ = enable;
		}
	}

	bool
	SpotLight::getShadowEnable() const noexcept
	{
		return this->shadowEnable_;
	}

	void
	SpotLight::setShadowBias(float bias) noexcept
	{
		shadowBias_ = bias;
	}

	float
	SpotLight::getShadowBias() const noexcept
	{
		return shadowBias_;
	}

	void
	SpotLight::setCamera(const std::shared_ptr<camera::Camera>& camera) noexcept
	{
		this->shadowCamera_ = camera;
	}

	const std::shared_ptr<camera::Camera>&
	SpotLight::getCamera() const noexcept
	{
		return this->shadowCamera_;
	}

	std::shared_ptr<video::RenderObject>
	SpotLight::clone() const noexcept
	{
		auto light = std::make_shared<SpotLight>();
		light->setInnerCone(this->getInnerCone().x);
		light->setOuterCone(this->getOuterCone().x);

		return light;
	}
}