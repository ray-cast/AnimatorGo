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
		:static_friction(0.5f), dynamic_friction(0.3f), restitution(0.1f),
		material(nullptr)
	{
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		
		material = physics_feature->getSDK()->createMaterial(static_friction, dynamic_friction, restitution);     //static friction, dynamic friction, restitution
		if (!material)
			runtime::runtime_error::create("createMaterial failed!");
	}

	PhysicsMaterial::PhysicsMaterial(const PhysicsMaterial& other) noexcept
	{
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();

		static_friction = other.static_friction;
		dynamic_friction = other.dynamic_friction;
		restitution = other.restitution;

		material = physics_feature->getSDK()->createMaterial(static_friction, dynamic_friction, restitution);     //static friction, dynamic friction, restitution
		if (!material)
			runtime::runtime_error::create("createMaterial failed!");
	}

	PhysicsMaterial::PhysicsMaterial(PhysicsMaterial&& other) noexcept
	{
		static_friction = other.static_friction;
		dynamic_friction = other.dynamic_friction;
		restitution = other.restitution;
		material = other.material;
		other.material = nullptr;
	}

	PhysicsMaterial::~PhysicsMaterial()
	{
		if (material)
		{
			material->release();
			material = nullptr;
		}
	}

    void PhysicsMaterial::setStaticFriction(float f) noexcept
    {
		static_friction = f;
		if (material)
		{
			material->setStaticFriction(f);
		}
    }

    float PhysicsMaterial::getStaticFriction() const noexcept
    {
        return static_friction;
    }

    void PhysicsMaterial::setDynamicFriction(float f) noexcept
    {
        f = dynamic_friction;
		if (material)
		{
			material->setDynamicFriction(f);
		}
    }

    float PhysicsMaterial::getDynamicFriction() const noexcept
    {
        return dynamic_friction;
    }

	void PhysicsMaterial::setRestitution(float r) noexcept
	{
		restitution = r;
		if (material)
		{
			material->setRestitution(r);
		}
	}

	float PhysicsMaterial::getRestitution() const noexcept
	{
		return restitution;
	}
}