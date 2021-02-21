#include <octoon/light/tube_light.h>

namespace octoon
{
	OctoonImplementSubClass(TubeLight, Light, "TubeLight")

	TubeLight::TubeLight() noexcept
	{
	}

	TubeLight::~TubeLight() noexcept
	{
	}

	std::shared_ptr<RenderObject>
	TubeLight::clone() const noexcept
	{
		auto light = std::make_shared<TubeLight>();
		return light;
	}
}