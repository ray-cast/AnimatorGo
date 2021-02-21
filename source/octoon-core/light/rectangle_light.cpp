#include <octoon/light/rectangle_light.h>

namespace octoon
{
	OctoonImplementSubClass(RectangleLight, Light, "RectangleLight")

	RectangleLight::RectangleLight() noexcept
	{
	}

	RectangleLight::~RectangleLight() noexcept
	{
	}

	std::shared_ptr<RenderObject>
	RectangleLight::clone() const noexcept
	{
		auto light = std::make_shared<RectangleLight>();
		return light;
	}
}