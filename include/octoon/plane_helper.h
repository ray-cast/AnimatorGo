#ifndef OCTOON_PLANE_HELPER_H_
#define OCTOON_PLANE_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT PlaneHelper final
	{
	public:
		static GameObjectPtr create(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept(false);
	};
}

#endif