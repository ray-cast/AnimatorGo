#include <octoon/physics_material.h>
#include <octoon/physics.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>


namespace octoon
{
    OctoonImplementSubClass(PhysicsMaterial, runtime::RttiInterface, "PhysicsMaterial")

	PhysicsMaterial::PhysicsMaterial() noexcept
	{

	}

	PhysicsMaterial::~PhysicsMaterial()
	{

	}

    void PhysicsMaterial::setBounciness(float b) noexcept
    {
        bounciness = b;
    }

    float PhysicsMaterial::getBounciness() const noexcept
    {
        return bounciness;
    }

    void PhysicsMaterial::setFriction(float f) noexcept
    {
        f = friction;
    }

    float PhysicsMaterial::getFriction() const noexcept
    {
        return friction;
    }
}