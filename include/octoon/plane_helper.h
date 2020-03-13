#ifndef OCTOON_PLANE_HELPER_H_
#define OCTOON_PLANE_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT PlaneHelper final
	{
	public:
		static GameObjectPtr create(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept(false);
		static GameObjectPtr create(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir) noexcept(false);
	};
}

#endif