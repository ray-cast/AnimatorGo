#ifndef OCTOON_SPHERE_HELPER_H_
#define OCTOON_SPHERE_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT SphereHelper final
	{
	public:
		static GameObjectPtr create(float radius, std::uint32_t widthSegments = 32, std::uint32_t heightSegments = 24, float phiStart = 0, float phiLength = math::PI_2, float thetaStart = 0, float thetaLength = math::PI) noexcept(false);
	};
}

#endif