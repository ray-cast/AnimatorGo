#include <octoon/light/tube_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(TubeLight, Light, "TubeLight")

	TubeLight::TubeLight() noexcept
	{
	}

	TubeLight::~TubeLight() noexcept
	{
	}

	video::RenderObjectPtr
	TubeLight::clone() const noexcept
	{
		auto light = std::make_shared<TubeLight>();
		return light;
	}
}