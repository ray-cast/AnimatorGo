#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
        PhysxContext::PhysxContext() noexcept
            :foundation(nullptr),
            defaultAllocatorCallback(new physx::PxDefaultAllocator),
            defaultErrorCallback(new physx::PxDefaultErrorCallback)
		{
            foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocatorCallback, *defaultErrorCallback);
            // if(!foundation) fatalError("PxCreateFoundation failed!");
            bool recordMemoryAllocations = true;

            pvd = PxCreatePvd(*foundation);
            physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
            pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

            physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
                physx::PxTolerancesScale(), recordMemoryAllocations, pvd);
            // if(!mPhysics) fatalError("PxCreatePhysics failed!");
		}

		PhysxContext::~PhysxContext() noexcept
		{
		}
	}
}
