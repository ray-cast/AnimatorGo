#include <octoon/collider_component.h>


namespace octoon
{
	OctoonImplementSubInterface(Collider, GameComponent, "Collider")

    Collider::Collider() noexcept
        :collider(nullptr)
    {

    }

    Collider::~Collider()
    {

    }

    void Collider::setBounciness(float b) noexcept
    {
        bounciness = b;
        if(collider)
        {
            collider->SetRestitution(bounciness);
        }
    }

    float Collider::getBounciness() const noexcept
    {
        return bounciness;
    }

    void Collider::setDensity(float d) noexcept
    {
        density = d;
        if(collider)
        {
            collider->SetDensity(density);
        }
    }

    float Collider::getDensity() const noexcept
    {
        return density;
    }

    void Collider::setFriction(float f) noexcept
    {
        friction = f;
        if(collider)
        {
            collider->SetFriction(friction);
        }
    }

    float Collider::getFriction() const noexcept
    {
        return friction;
    }
}