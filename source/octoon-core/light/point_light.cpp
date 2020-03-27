#include <octoon/light/point_light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon::light
{
	OctoonImplementSubClass(PointLight, Light, "PointLight")

	PointLight::PointLight() noexcept
		: _enableSoftShadow(false)
		, _shadowMode(ShadowMode::ShadowModeNone)
		, _shadowBias(0.1f)
		, _shadowFactor(600.0f)
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
	PointLight::setShadowMode(ShadowMode shadowMode) noexcept
	{
		if (_shadowMode != shadowMode)
		{
			_shadowMode = shadowMode;
		}
	}

	ShadowMode
	PointLight::getShadowMode() const noexcept
	{
		return _shadowMode;
	}

	const std::shared_ptr<camera::Camera>&
	PointLight::getCamera() const noexcept
	{
		return _shadowCamera;
	}

	void
	PointLight::setShadowBias(float bias) noexcept
	{
		_shadowBias = bias;
	}

	void
	PointLight::setShadowFactor(float factor) noexcept
	{
		_shadowFactor = factor;
	}

	float
	PointLight::getShadowBias() const noexcept
	{
		return _shadowBias;
	}

	float
	PointLight::getShadowFactor() const noexcept
	{
		return _shadowFactor;
	}

	video::RenderObjectPtr
	PointLight::clone() const noexcept
	{
		auto light = std::make_shared<PointLight>();

		return light;
	}
}