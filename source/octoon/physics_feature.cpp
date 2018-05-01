#include <stdexcept>

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
		:dispatcher(nullptr), physicsScene(nullptr),
		accumulator(0.0f), stepSize(1.0f / 60.0f)
	{
		foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (!foundation)
			throw std::runtime_error("PxCreateFoundation failed!");

		bool recordMemoryAllocations = true;

		pvd = physx::PxCreatePvd(*foundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
			physx::PxTolerancesScale(), recordMemoryAllocations, pvd);
		if (!physics)
			throw std::runtime_error("PxCreatePhysics failed!");

		physx::PxTolerancesScale scale;
		scale.length = 1;
		scale.speed = 1;

		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(scale));
		if (!cooking)
			throw std::runtime_error("PxCreateCooking failed!");


		if (!PxInitExtensions(*physics, pvd))
			throw std::runtime_error("PxInitExtensions failed!");


		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = dispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		physicsScene = physics->createScene(sceneDesc);
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
		physics->release();
		foundation->release();
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