#include <octoon/collider_component.h>


namespace octoon
{
	OctoonImplementSubInterface(Collider, GameComponent, "Collider")

    Collider::Collider() noexcept
    {

    }

    Collider::~Collider()
    {

    }

    void Collider::setBounciness(float b) noexcept
    {
        bounciness = b;
    }

    float Collider::getBounciness() const noexcept
    {
        return bounciness;
    }

    void Collider::setDensity(float d) noexcept
    {
        density = d;
    }

    float Collider::getDensity() const noexcept
    {
        return density;
    }

    void Collider::setFriction(float f) noexcept
    {
        friction = f;
    }

    float Collider::getFriction() const noexcept
    {
        return friction;
    }
}