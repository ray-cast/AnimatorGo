#include <octoon/light/disk_light.h>

namespace octoon
{
	OctoonImplementSubClass(DiskLight, Light, "DiskLight")

	DiskLight::DiskLight() noexcept
	{
	}

	DiskLight::~DiskLight() noexcept
	{
	}

	std::shared_ptr<RenderObject>
	DiskLight::clone() const noexcept
	{
		auto light = std::make_shared<DiskLight>();
		return light;
	}
}