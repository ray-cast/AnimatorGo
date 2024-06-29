#include <octoon/light/directional_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(DirectionalLight, Light, "DirectionalLight")

	DirectionalLight::DirectionalLight() noexcept
		: shadowBias_(0.0f)
		, shadowRadius_(0.0f)
	{
	}

	DirectionalLight::~DirectionalLight() noexcept
	{
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

	std::shared_ptr<video::RenderObject>
	DirectionalLight::clone() const noexcept
	{
		auto light = std::make_shared<DirectionalLight>();
		light->setShadowBias(this->getShadowBias());
		light->setShadowRadius(this->getShadowRadius());
		return light;
	}
}