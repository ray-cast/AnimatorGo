#include <stdexcept>

#include <octoon/game_object.h>
#include <octoon/transform_component.h>
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
		:defaultErrorCallback(std::make_unique<physx::PxDefaultErrorCallback>()),
		defaultAllocatorCallback(std::make_unique<physx::PxDefaultAllocator>()),
		foundation(nullptr), pvd(nullptr),
		physics(nullptr), cooking(nullptr),
		dispatcher(nullptr), physicsScene(nullptr),
		accumulator(0.0f), stepSize(1.0f / 60.0f)
	{
		foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *defaultAllocatorCallback, *defaultErrorCallback);
		if (!foundation)
			throw runtime::runtime_error::create("PxCreateFoundation failed!");

		bool recordMemoryAllocations = true;

		pvd = physx::PxCreatePvd(*foundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
			physx::PxTolerancesScale(), recordMemoryAllocations, pvd);
		if (!physics)
			throw runtime::runtime_error::create("PxCreatePhysics failed!");

		physx::PxTolerancesScale scale;
		scale.length = 1;
		scale.speed = 1;

		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(scale));
		if (!cooking)
			throw runtime::runtime_error::create("PxCreateCooking failed!");

		if (!PxInitExtensions(*physics, pvd))
			throw runtime::runtime_error::create("PxInitExtensions failed!");

		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
		Physics::setGravity(math::Vector3(0.f, -9.8f, 0.f));
		math::Vector3 g = Physics::getGravity();
		sceneDesc.gravity = physx::PxVec3(g.x, g.y, g.z);
		dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = dispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		physicsScene = physics->createScene(sceneDesc);
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
		physicsScene->release();
		cooking->release();
		physics->release();
		pvd->release();
		foundation->release();
		physicsScene = nullptr;
		cooking = nullptr;
		physics = nullptr;
		pvd = nullptr;
		foundation = nullptr;
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
		if (physicsScene)
		{
			auto delta = this->getFeature<TimerFeature>()->delta();
			accumulator += delta;
			if (accumulator < stepSize)	return;
			accumulator -= stepSize;

			physicsScene->simulate(stepSize);
			physicsScene->fetchResults(true);
		}
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