#include <octoon/light/directional_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(DirectionalLight, Light, "DirectionalLight")

	DirectionalLight::DirectionalLight() noexcept
	{
	}

	DirectionalLight::~DirectionalLight() noexcept
	{
	}

	std::shared_ptr<video::RenderObject>
	DirectionalLight::clone() const noexcept
	{
		auto light = std::make_shared<DirectionalLight>();
		return light;
	}
}