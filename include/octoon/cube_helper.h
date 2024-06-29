#ifndef OCTOON_CUBE_HELPER_H_
#define OCTOON_CUBE_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT CubeHelper final
	{
	public:
		static GameObjectPtr create(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept(false);
	};
}

#endif