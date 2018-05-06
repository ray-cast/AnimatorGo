#ifndef OCTOON_JOINT_SPRING_H_
#define OCTOON_JOINT_SPRING_H_

#include <octoon/math/math.h>

namespace octoon
{
    class JointSpring
    {
    public:     
        float damper; // The damper force uses to dampen the spring.
        float spring; // The spring forces used to reach the target position.
        math::float3 targetPosition; // The target position the joint attempts to reach.

		JointSpring()noexcept
			: damper(0.3f), spring(1.0f), targetPosition()
		{
		}
    };
}



#endif // OCTOON_JOINT_SPRING_H_