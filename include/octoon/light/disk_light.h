#ifndef OCTOON_DISK_LIGHT_H_
#define OCTOON_DISK_LIGHT_H_

#include <octoon/light/light.h>

namespace octoon::light
{
	class OCTOON_EXPORT DiskLight final : public Light
	{
		OctoonDeclareSubClass(DiskLight, Light)
	public:
		DiskLight() noexcept;
		virtual ~DiskLight() noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		DiskLight(const DiskLight&) noexcept = delete;
		DiskLight& operator=(const DiskLight&) noexcept = delete;
	};
}

#endif