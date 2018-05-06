#ifndef OCTOON_JOINT_MOTOR_H_
#define OCTOON_JOINT_MOTOR_H_

#include <octoon/math/math.h>

namespace octoon
{
    class JointMotor
    {
    public:
		float force; // The motor will apply a force.
		bool freeSpin; // If freeSpin is enabled the motor will only accelerate but never slow down.
		float targetVelocity; // The motor will apply a force up to force to achieve targetVelocity.

		JointMotor() noexcept
			: force(0.3f), freeSpin(false), targetVelocity(1.0f)
		{
		}
    };
}



#endif // OCTOON_JOINT_MOTOR_H_