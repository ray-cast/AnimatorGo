#include <octoon/light/ambient_light.h>

namespace octoon
{
	OctoonImplementSubClass(AmbientLight, Light, "AmbientLight")

	AmbientLight::AmbientLight() noexcept
	{
	}

	AmbientLight::~AmbientLight() noexcept
	{
	}

	std::shared_ptr<RenderObject>
	AmbientLight::clone() const noexcept
	{
		auto light = std::make_shared<AmbientLight>();
		return light;
	}
}