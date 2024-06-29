#ifndef OCTOON_PHYSX_CONTEXT_H_
#define OCTOON_PHYSX_CONTEXT_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_shape.h>
#include <octoon/physics/physics_sphere_shape.h>
#include <octoon/physics/physics_joint.h>
#include <octoon/physics/physics_fixed_joint.h>
#include <octoon/physics/physics_configurable_joint.h>

#include "physx_scene.h"
#include "physx_type.h"

namespace octoon
{
	class OCTOON_EXPORT PhysxContext : public PhysicsContext
	{
	public:
		PhysxContext();
		virtual ~PhysxContext();

		virtual std::unique_ptr<PhysicsScene> createScene(const PhysicsSceneDesc& desc) override;
		virtual std::unique_ptr<PhysicsRigidbody> createRigidbody(const PhysicsRigidbodyDesc& desc) override;
		virtual std::unique_ptr<PhysicsSphereShape> createSphereShape(const PhysicsSphereShapeDesc& desc) override;
		virtual std::unique_ptr<PhysicsBoxShape> createBoxShape(const PhysicsBoxShapeDesc& desc) override;
		virtual std::unique_ptr<PhysicsCapsuleShape> createCapsuleShape(const PhysicsCapsuleShapeDesc& desc) override;
		virtual std::unique_ptr<PhysicsFixedJoint> createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;
		virtual std::unique_ptr<PhysicsConfigurableJoint> createConfigurableJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;

		physx::PxPhysics* getPxPhysics();
		nv::cloth::Factory* getClothFactory();

	private:
		PhysxContext(const PhysxContext&) noexcept = delete;
		PhysxContext& operator=(const PhysxContext&) noexcept = delete;
    private:
		nv::cloth::Factory* factory_;
        physx::PxFoundation* foundation;
        physx::PxPvd* pvd;
        physx::PxPhysics* physics;
        physx::PxCooking* cooking;
		physx::PxPvdTransport* transport_;
        std::unique_ptr<physx::PxErrorCallback> defaultErrorCallback;
        std::unique_ptr<physx::PxDefaultAllocator> defaultAllocatorCallback;
	};
}

#endif