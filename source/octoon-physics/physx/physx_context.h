#ifndef OCTOON_PHYSX_CONTEXT_H_
#define OCTOON_PHYSX_CONTEXT_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_shape.h>
#include <octoon/physics/physics_sphere_shape.h>
#include <octoon/physics/physics_joint.h>
#include <octoon/physics/physics_fixed_joint.h>

#include "physx_scene.h"

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxContext : public PhysicsContext
		{
		public:
			PhysxContext();
			virtual ~PhysxContext();

			virtual std::shared_ptr<PhysicsScene> createScene(PhysicsSceneDesc desc) override;
			virtual std::shared_ptr<PhysicsRigidbody> createRigidbody(PhysicsRigidbodyDesc desc) override;
			virtual std::shared_ptr<PhysicsSphereShape> createShape(PhysicsSphereShapeDesc desc) override;
			virtual std::shared_ptr<PhysicsFixedJoint> createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;

			physx::PxPhysics* getPxPhysics();
			physx::PxDefaultCpuDispatcher* getPxDefaultCpuDispatcher();

		private:
			PhysxContext(const PhysxContext&) noexcept = delete;
			PhysxContext& operator=(const PhysxContext&) noexcept = delete;
        private:
            physx::PxFoundation* foundation;
            physx::PxPvd* pvd;
            physx::PxPhysics* physics;
            physx::PxCooking* cooking;
            physx::PxDefaultCpuDispatcher* dispatcher;
            std::unique_ptr<physx::PxDefaultErrorCallback> defaultErrorCallback;
            std::unique_ptr<physx::PxDefaultAllocator> defaultAllocatorCallback;
		};
	}
}

#endif