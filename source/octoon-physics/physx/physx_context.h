#ifndef OCTOON_PHYSX_CONTEXT_H_
#define OCTOON_PHYSX_CONTEXT_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_context.h>

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxContext : public PhysicsContext
		{
		public:
			PhysxContext() noexcept;
			virtual ~PhysxContext();

		private:
			PhysxContext(const PhysxContext&) noexcept = delete;
			PhysxContext& operator=(const PhysxContext&) noexcept = delete;
        private:
            physx::PxFoundation* foundation;
            physx::PxPvd* pvd;
            physx::PxPhysics* physics;
            std::unique_ptr<physx::PxDefaultErrorCallback> defaultErrorCallback;
            std::unique_ptr<physx::PxDefaultAllocator> defaultAllocatorCallback;
		};
	}
}

#endif