#include <stdexcept>

#include <octoon/physics_feature.h>
#include <octoon/physics.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>

#include <PxPhysicsAPI.h>


namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")
    
	PhysicsFeature::PhysicsFeature() except
		:dispatcher(nullptr), physicsScene(nullptr),
		accumulator(0.0f), stepSize(1.0f / 60.0f)
	{
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

    void PhysicsFeature::onActivate() except
    {
		foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (!foundation)
			runtime::runtime_error::create("PxCreateFoundation failed!");

		bool recordMemoryAllocations = true;

		pvd = physx::PxCreatePvd(*foundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
			physx::PxTolerancesScale(), recordMemoryAllocations, pvd);
		if (!physics)
			runtime::runtime_error::create("PxCreatePhysics failed!");

		physx::PxTolerancesScale scale;
		scale.length = 1;
		scale.speed = 1;

		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(scale));
		if (!cooking)
			runtime::runtime_error::create("PxCreateCooking failed!");


		if (!PxInitExtensions(*physics, pvd))
			runtime::runtime_error::create("PxInitExtensions failed!");


		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
		math::Vector3 g = Physics::getGravity();
		sceneDesc.gravity = physx::PxVec3(g.x, g.y, g.z);
		dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = dispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		physicsScene = physics->createScene(sceneDesc);
    }

    void PhysicsFeature::onDeactivate() noexcept
    {
		physics->release();
		foundation->release();
    }

    void PhysicsFeature::onInputEvent(const input::InputEvent& event) noexcept
    {

    }

    void PhysicsFeature::onReset() noexcept
    {

    }

    void PhysicsFeature::onFrameBegin() noexcept
    {
		auto delta = runtime::Singleton<GameApp>::instance()->getFeature<TimerFeature>()->delta();
		accumulator += delta;
		if (accumulator < stepSize)	return;
		accumulator -= stepSize;
		physicsScene->simulate(stepSize);
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