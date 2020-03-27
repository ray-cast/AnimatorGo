#include <octoon/light/disk_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(DiskLight, Light, "DiskLight")

	DiskLight::DiskLight() noexcept
	{
	}

	DiskLight::~DiskLight() noexcept
	{
	}

	video::RenderObjectPtr
	DiskLight::clone() const noexcept
	{
		auto light = std::make_shared<DiskLight>();
		return light;
	}
}