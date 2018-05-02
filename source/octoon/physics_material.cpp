#include <octoon/physics_material.h>
#include <octoon/physics.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>

#include <PxPhysicsAPI.h>


namespace octoon
{
    OctoonImplementSubClass(PhysicsMaterial, runtime::RttiInterface, "PhysicsMaterial")

	PhysicsMaterial::PhysicsMaterial() noexcept
	{
		auto physics_feature = runtime::Singleton<GameApp>::instance()->getFeature<PhysicsFeature>();
		
		material = physics_feature->getSDK()->createMaterial(friction, friction, bounciness);     //static friction, dynamic friction, restitution
		if (!material)
			runtime::runtime_error::create("createMaterial failed!");
	}

	PhysicsMaterial::~PhysicsMaterial()
	{
		material->release();
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