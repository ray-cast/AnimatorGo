#ifndef OCTOON_PHYSX_CONTEXT_H_
#define OCTOON_PHYSX_CONTEXT_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_context.h>

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
			virtual std::shared_ptr<PhysicsRigidbody> createRigidbody() override;

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