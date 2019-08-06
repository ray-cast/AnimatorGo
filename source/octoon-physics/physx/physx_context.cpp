#include "physx_context.h"
#include "physx_rigidbody.h"
#include "physx_shape.h"
#include "physx_box_shape.h"
#include "physx_sphere_shape.h"
#include "physx_capsule_shape.h"
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
			: foundation(nullptr)
			, pvd(nullptr)
			, physics(nullptr)
			, cooking(nullptr)
			, defaultAllocatorCallback(std::make_unique<physx::PxDefaultAllocator>())
			, defaultErrorCallback(std::make_unique<physx::PxDefaultErrorCallback>())
			, transport_(nullptr)
		{
			physx::PxTolerancesScale scale;
			scale.length = 1;
			scale.speed = 9.8f;

			foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocatorCallback, *defaultErrorCallback);
			if (!foundation)
				throw runtime::runtime_error::create("PxCreateFoundation failed!");

			transport_ = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
			pvd = physx::PxCreatePvd(*foundation);
			pvd->connect(*transport_, physx::PxPvdInstrumentationFlag::eALL);

			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, true, pvd);
			if (!physics)
				 throw runtime::runtime_error::create("PxCreatePhysics failed!");

			cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(scale));
			if (!cooking)
				throw runtime::runtime_error::create("PxCreateCooking failed!");

			if (!PxInitExtensions(*physics, pvd))
				throw runtime::runtime_error::create("PxInitExtensions failed!");
		}

		PhysxContext::~PhysxContext()
		{
			PxCloseExtensions();

			if (physics)
				physics->release();

			if (cooking)
				cooking->release();

			if (pvd)
				pvd->release();

			if (transport_)
				transport_->release();

			if (foundation)
				foundation->release();
		}

		std::shared_ptr<PhysicsScene> PhysxContext::createScene(PhysicsSceneDesc desc)
		{
			return std::make_shared<PhysxScene>(this, desc);
		}

		std::shared_ptr<PhysicsRigidbody> PhysxContext::createRigidbody(PhysicsRigidbodyDesc desc)
		{
			return std::make_shared<PhysxRigidbody>(this, desc);
		}

		std::shared_ptr<PhysicsBoxShape> PhysxContext::createBoxShape(PhysicsBoxShapeDesc desc)
		{
			return std::make_shared<PhysxBoxShape>(this, desc.width, desc.height, desc.depth);
		}

		std::shared_ptr<PhysicsSphereShape> PhysxContext::createSphereShape(PhysicsSphereShapeDesc desc)
		{
			return std::make_shared<PhysxSphereShape>(this, desc.radius);
		}

		std::shared_ptr<PhysicsCapsuleShape> PhysxContext::createCapsuleShape(PhysicsCapsuleShapeDesc desc)
		{
			return std::make_shared<PhysxCapsuleShape>(this, desc.radius, desc.height);
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
	}
}
