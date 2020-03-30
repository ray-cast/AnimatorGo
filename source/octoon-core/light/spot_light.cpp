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

	std::shared_ptr<video::RenderObject>
	SpotLight::clone() const noexcept
	{
		auto light = std::make_shared<SpotLight>();
		light->setInnerCone(this->getInnerCone().x);
		light->setOuterCone(this->getOuterCone().x);

		return light;
	}
}