#include <octoon/light/rectangle_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(RectangleLight, Light, "RectangleLight")

	RectangleLight::RectangleLight() noexcept
	{
	}

	RectangleLight::~RectangleLight() noexcept
	{
	}

	std::shared_ptr<video::RenderObject>
	RectangleLight::clone() const noexcept
	{
		auto light = std::make_shared<RectangleLight>();
		return light;
	}
}