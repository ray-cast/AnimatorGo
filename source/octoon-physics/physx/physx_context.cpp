#include "physx_context.h"


#include "physx_rigidbody.h"
#include "physx_shape.h"
#include "physx_sphere_shape.h"
#include "physx_fixed_joint.h"
#include "physx_configurable_joint.h"

#include <octoon/runtime/except.h>
#include <octoon/math/math.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
        PhysxContext::PhysxContext()
            :foundation(nullptr),
			pvd(nullptr),
			physics(nullptr),
			cooking(nullptr),
            defaultAllocatorCallback(new physx::PxDefaultAllocator),
            defaultErrorCallback(new physx::PxDefaultErrorCallback)
		{
			// create foundation
            foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocatorCallback, *defaultErrorCallback);
            if (!foundation)
                throw runtime::runtime_error::create("PxCreateFoundation failed!");

			// create pvd
            pvd = physx::PxCreatePvd(*foundation);
            physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
            pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

			// create physics
			bool recordMemoryAllocations = true;
            physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
                physx::PxTolerancesScale(), recordMemoryAllocations, pvd);
            if (!physics)
                throw runtime::runtime_error::create("PxCreatePhysics failed!");

			// create cooking
            physx::PxTolerancesScale scale;
            scale.length = 1;
            scale.speed = 1;

            cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(scale));
            if (!cooking)
                throw runtime::runtime_error::create("PxCreateCooking failed!");

            if (!PxInitExtensions(*physics, pvd))
                throw runtime::runtime_error::create("PxInitExtensions failed!");

			// create dispatcher
			dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		}

		PhysxContext::~PhysxContext()
		{
			if (physics)
			{
				physics->release();
				physics = nullptr;
			}
		}

        std::shared_ptr<PhysicsScene> PhysxContext::createScene(PhysicsSceneDesc desc)
        {
			auto scene_obj = std::make_shared<PhysxScene>(this, desc);
			return scene_obj;
        }

		std::shared_ptr<PhysicsRigidbody> PhysxContext::createRigidbody(PhysicsRigidbodyDesc desc)
		{
			return std::make_shared<PhysxRigidbody>(this, desc);
		}

		std::shared_ptr<PhysicsSphereShape> PhysxContext::createShape(PhysicsSphereShapeDesc desc)
		{
			return std::make_shared<PhysxSphereShape>(this, desc.radius);
		}

		std::shared_ptr<PhysicsFixedJoint> PhysxContext::createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
		{
			auto ret = std::make_shared<PhysxFixedJoint>(this);
			ret->connect(lhs, rhs);
			return ret;
		}

		std::shared_ptr<PhysicsConfigurableJoint> PhysxContext::createConfigurableJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
		{
			auto ret = std::make_shared<PhysxConfigurableJoint>(this);
			ret->connect(lhs, rhs);
			return ret;
		}

		physx::PxPhysics * PhysxContext::getPxPhysics()
		{
			return physics;
		}
		physx::PxDefaultCpuDispatcher * PhysxContext::getPxDefaultCpuDispatcher()
		{
			return dispatcher;
		}
	}
}
