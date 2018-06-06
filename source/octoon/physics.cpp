#include <octoon/physics.h>

namespace octoon
{
	math::Vector3 Physics::gravity;

    void Physics::setGravity(const math::Vector3& g)
    {
        gravity=g;
    }

    math::Vector3 Physics::getGravity()
    {
        return gravity;
    }
}