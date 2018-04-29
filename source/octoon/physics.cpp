#include <octoon/physics.h>

namespace octoon
{
	math::Vector2 Physics::gravity;

    void Physics::setGravity(const math::Vector2& g)
    {
        gravity=g;
    }

    math::Vector2 Physics::getGravity()
    {
        return gravity;
    }
}