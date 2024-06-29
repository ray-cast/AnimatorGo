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
#include <NvCloth/Factory.h>

namespace octoon
{
	PhysxContext::PhysxContext()
		: foundation(nullptr)
		, pvd(nullptr)
		, physics(nullptr)
		, cooking(nullptr)
		, defaultAllocatorCallback(std::make_unique<physx::PxDefaultAllocator>())
		, defaultErrorCallback(std::make_unique<physx::PxDefaultErrorCallback>())
		, transport_(nullptr)
		, factory_(nullptr)
	{
		physx::PxTolerancesScale scale;
		scale.length = 10.0f;
		scale.speed = 9.8f;

		foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocatorCallback, *defaultErrorCallback);
		if (!foundation)
			throw runtime_error::create("PxCreateFoundation failed!");

		transport_ = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
		pvd = physx::PxCreatePvd(*foundation);
		pvd->connect(*transport_, physx::PxPvdInstrumentationFlag::eALL);

		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, true, pvd);
		if (!physics)
				throw runtime_error::create("PxCreatePhysics failed!");

		// FIXME
		// cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(scale));
		// if (!cooking)
		// 	throw runtime_error::create("PxCreateCooking failed!");

		if (!PxInitExtensions(*physics, pvd))
			throw runtime_error::create("PxInitExtensions failed!");
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

	std::unique_ptr<PhysicsScene>
	PhysxContext::createScene(const PhysicsSceneDesc& desc)
	{
		return std::make_unique<PhysxScene>(this, desc);
	}

	std::unique_ptr<PhysicsRigidbody>
	PhysxContext::createRigidbody(const PhysicsRigidbodyDesc& desc)
	{
		return std::make_unique<PhysxRigidbody>(this, desc);
	}

	std::unique_ptr<PhysicsBoxShape>
	PhysxContext::createBoxShape(const PhysicsBoxShapeDesc& desc)
	{
		return std::make_unique<PhysxBoxShape>(this, desc.width, desc.height, desc.depth);
	}

	std::unique_ptr<PhysicsSphereShape>
	PhysxContext::createSphereShape(const PhysicsSphereShapeDesc& desc)
	{
		return std::make_unique<PhysxSphereShape>(this, desc.radius);
	}

	std::unique_ptr<PhysicsCapsuleShape>
	PhysxContext::createCapsuleShape(const PhysicsCapsuleShapeDesc& desc)
	{
		return std::make_unique<PhysxCapsuleShape>(this, desc.radius, desc.height);
	}

	std::unique_ptr<PhysicsFixedJoint>
	PhysxContext::createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		auto ret = std::make_unique<PhysxFixedJoint>(this);
		ret->connect(lhs, rhs);
		return ret;
	}

	std::unique_ptr<PhysicsConfigurableJoint>
	PhysxContext::createConfigurableJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		auto ret = std::make_unique<PhysxConfigurableJoint>(this);
		ret->connect(lhs, rhs);
		return ret;
	}

	physx::PxPhysics*
	PhysxContext::getPxPhysics()
	{
		return physics;
	}

	nv::cloth::Factory*
	PhysxContext::getClothFactory()
	{
		return factory_;
	}
}
