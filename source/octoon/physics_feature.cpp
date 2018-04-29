#include <octoon/physics_feature.h>
#include <octoon/physics.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>


namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")
    
	PhysicsFeature::PhysicsFeature() noexcept
	{
		foundation = physx::PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (!foundation)
			physx::fatalError("PxCreateFoundation failed!");

		bool recordMemoryAllocations = true;

		pvd = physx::PxCreatePvd(*foundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		physics = physx::PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
			physx::PxTolerancesScale(), recordMemoryAllocations, pvd);
		if (!physics)
			physx::fatalError("PxCreatePhysics failed!");

		cooking = physx::PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(scale));
		if (!cooking)
			physx::fatalError("PxCreateCooking failed!");


		if (!physx::PxInitExtensions(*physics, pvd))
			physx::fatalError("PxInitExtensions failed!");

	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{

	}

    void PhysicsFeature::onActivate() except
    {
    }

    void PhysicsFeature::onDeactivate() noexcept
    {

    }

    void PhysicsFeature::onInputEvent(const input::InputEvent& event) noexcept
    {

    }

    void PhysicsFeature::onReset() noexcept
    {

    }

    void PhysicsFeature::onFrameBegin() noexcept
    {

    }

    void PhysicsFeature::onFrame() except
    {

    }

    void PhysicsFeature::onFrameEnd() noexcept
    {

    }

    void PhysicsFeature::onOpenScene(const GameScenePtr& scene) except
    {

    }

	void PhysicsFeature::onCloseScene(const GameScenePtr& scene) noexcept
    {

    }

}