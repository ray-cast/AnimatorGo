#ifndef OCTOON_CONE_HELPER_H_
#define OCTOON_CONE_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT ConeHelper final
	{
	public:
		static GameObjectPtr create(float radius, float height, std::uint32_t segments = 32, float thetaStart = 0, float thetaLength = math::PI_2) noexcept(false);
	};
}

#endif