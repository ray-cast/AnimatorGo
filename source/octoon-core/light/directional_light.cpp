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

	video::RenderObjectPtr
	DirectionalLight::clone() const noexcept
	{
		auto light = std::make_shared<DirectionalLight>();
		return light;
	}
}