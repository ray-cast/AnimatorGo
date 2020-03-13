#ifndef OCTOON_RING_HELPER_H_
#define OCTOON_RING_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT RingHelper final
	{
	public:
		static GameObjectPtr create(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart = 0, float thetaLength = math::PI) noexcept(false);
	};
}

#endif