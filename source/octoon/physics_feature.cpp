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
		dispatcher(nullptr), physicsScene(nullptr),
		accumulator(0.0f), stepSize(1.0f / 60.0f)
	{
		foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *defaultAllocatorCallback, *defaultErrorCallback);
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
		Physics::setGravity(math::Vector3(0.f, -9.f, 0.f));
		math::Vector3 g = Physics::getGravity();
		sceneDesc.gravity = physx::PxVec3(g.x, g.y, g.z);
		dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = dispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		physicsScene = physics->createScene(sceneDesc);
		physicsScene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

    void PhysicsFeature::onActivate() except
    {
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
		
    }

    void PhysicsFeature::onFrame() except
    {
		auto delta = this->getFeature<TimerFeature>()->delta();
		accumulator += delta / 360.f;
		if (accumulator < stepSize)	return;
		accumulator -= stepSize;
		
		physicsScene->simulate(stepSize);
		physicsScene->fetchResults(true);

		// retrieve array of actors that moved
		physx::PxU32 nbActiveActors;
		physx::PxActor** activeActors = physicsScene->getActiveActors(nbActiveActors);

		// update each render object with the new transform
		for (physx::PxU32 i = 0; i < nbActiveActors; ++i)
		{
			GameObject* renderObject = static_cast<GameObject*>(activeActors[i]->userData);
			physx::PxRigidActor* actor = static_cast<physx::PxRigidActor*>(activeActors[i]);

			physx::PxTransform transform = actor->getGlobalPose();
			auto transform_component = renderObject->getComponent<TransformComponent>();
			transform_component->setTranslate(math::Vector3(transform.p.x, transform.p.y, transform.p.z));
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