#ifndef OCTOON_CIRCLE_HELPER_H_
#define OCTOON_CIRCLE_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT CircleHelper final
	{
	public:
		static GameObjectPtr create(float radius, std::uint32_t segments, float thetaStart = 0, float thetaLength = math::PI_2) noexcept(false);
	};
}

#endif