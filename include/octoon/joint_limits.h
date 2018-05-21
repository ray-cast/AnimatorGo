#ifndef OCTOON_JOINT_LIMITS_H_
#define OCTOON_JOINT_LIMITS_H_

namespace octoon
{
    class JointLimits
    {
    public:
        float bounceMinVelocity; // The minimum impact velocity which will cause the joint to bounce.
        float bounciness; // Determines the size of the bounce when the joint hits it's limit. Also known as restitution.
        float contactDistance; // Distance inside the limit value at which the limit will be considered to be active by the solver.
        float max; // The upper angular limit (in degrees) of the joint.
        float min; // The lower angular limit (in degrees) of the joint.

		JointLimits()noexcept
			: bounceMinVelocity(0.1f), bounciness(0.1f)
			, contactDistance(3.0f), max(1.0f), min(0.f)
		{
		}
    };
}



#endif // OCTOON_JOINT_LIMITS_H_