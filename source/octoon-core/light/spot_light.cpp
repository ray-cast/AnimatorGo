#include <octoon/light/spot_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(SpotLight, Light, "SpotLight")

	SpotLight::SpotLight() noexcept
		: _spotInnerCone(5.0f, math::cos(math::radians(5.0f)))
		, _spotOuterCone(40.0f, math::cos(math::radians(40.0f)))
	{
	}

	SpotLight::~SpotLight() noexcept
	{
	}

	void
	SpotLight::setSpotInnerCone(float value) noexcept
	{
		_spotInnerCone.x = math::min(_spotOuterCone.x, value);
		_spotInnerCone.y = math::cos(math::radians(_spotInnerCone.x));
	}

	void
	SpotLight::setSpotOuterCone(float value) noexcept
	{
		_spotOuterCone.x = math::max(_spotInnerCone.x, value);
		_spotOuterCone.y = math::cos(math::radians(_spotOuterCone.x));
	}

	const math::float2&
	SpotLight::getSpotInnerCone() const noexcept
	{
		return _spotInnerCone;
	}

	const math::float2&
	SpotLight::getSpotOuterCone() const noexcept
	{
		return _spotOuterCone;
	}

	std::shared_ptr<video::RenderObject>
	SpotLight::clone() const noexcept
	{
		auto light = std::make_shared<SpotLight>();
		light->setSpotInnerCone(this->getSpotInnerCone().x);
		light->setSpotOuterCone(this->getSpotOuterCone().x);

		return light;
	}
}