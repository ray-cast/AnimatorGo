#include "physx_scene.h"
#include "physx_rigidbody.h"
#include "physx_context.h"

#include <omp.h>
#include <PxPhysicsAPI.h>

namespace octoon
{
	physx::PxFilterFlags DefaultPhysXSimulationFilterShader(
		physx::PxFilterObjectAttributes attributes0,
		physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1,
		physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags,
		const void* constantBlock,
		physx::PxU32 constantBlockSize)
	{
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlags();
		}

		if (!physx::PxGetGroupCollisionFlag((physx::PxU16)filterData0.word0, (physx::PxU16)filterData1.word0))
			return physx::PxFilterFlag::eSUPPRESS;

		bool collides = ((1 << filterData0.word0) & filterData1.word2) != 0;
		collides = collides && ((1 << filterData1.word0) & filterData0.word2);

		return collides ? physx::PxFilterFlag::eDEFAULT : physx::PxFilterFlag::eSUPPRESS;
	};

	class SimulationEventCallback : public physx::PxSimulationEventCallback
	{
	public:
		void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override
		{
		}

		void onWake(physx::PxActor** actors, physx::PxU32 count) override
		{
		}
			
		void onSleep(physx::PxActor** actors, physx::PxU32 count) override
		{
		}

		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override
		{
		}

		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override
		{
		}

		void onAdvance(const  physx::PxRigidBody* const* bodyBuffer, const  physx::PxTransform* poseBuffer, const  physx::PxU32 count) override
		{
		}
	};

	PhysxScene::PhysxScene(PhysxContext* context, PhysicsSceneDesc desc)
		: context(nullptr)
		, px_scene(nullptr)
		, simulationEventCallback_(std::make_unique<SimulationEventCallback>())
		, maxSubSteps_(1)
		, fixedTimeStep_(1.0f / 60.0f)
	{
		physx::PxSceneDesc sceneDesc(context->getPxPhysics()->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(desc.gravity.x, desc.gravity.y, desc.gravity.z);
		sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(omp_get_num_procs() / 2);
		sceneDesc.filterShader = DefaultPhysXSimulationFilterShader;
		sceneDesc.simulationEventCallback = simulationEventCallback_.get();
		sceneDesc.ccdMaxPasses = 4;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS | physx::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS;
		px_scene = context->getPxPhysics()->createScene(sceneDesc);

		auto pvdClient = px_scene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}

	PhysxScene::~PhysxScene()
	{
		px_scene->release();
		px_scene = nullptr;
	}

	void
	PhysxScene::setGroundEnable(bool value) noexcept
	{
	}

	bool
	PhysxScene::getGroundEnable() const noexcept
	{
		return false;
	}

	void
	PhysxScene::setGravity(const math::float3& gravity) noexcept
	{
		px_scene->setGravity(physx::PxVec3(gravity.x, gravity.y, gravity.z));
	}

	math::float3
	PhysxScene::getGravity() const noexcept
	{
		auto gravity = px_scene->getGravity();
		return math::float3(gravity.x, gravity.y, gravity.z);
	}

	void
	PhysxScene::addRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
	{
		std::shared_ptr<PhysxRigidbody> physx_rigidbody = std::dynamic_pointer_cast<PhysxRigidbody>(rigidbody);
		px_scene->addActor(*physx_rigidbody->getPxRigidbody());
	}

	void
	PhysxScene::removeRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
	{
		std::shared_ptr<PhysxRigidbody> physx_rigidbody = std::dynamic_pointer_cast<PhysxRigidbody>(rigidbody);
		px_scene->removeActor(*physx_rigidbody->getPxRigidbody());
	}

	void
	PhysxScene::addConstraint(std::shared_ptr<PhysicsJoint> joint)
	{
	}

	void
	PhysxScene::removeConstraint(std::shared_ptr<PhysicsJoint> joint)
	{
	}

	void
	PhysxScene::setMaxSubStepCount(int numSubSteps) noexcept
	{
		maxSubSteps_ = numSubSteps;
	}

	int
	PhysxScene::getMaxSubStepCount() noexcept
	{
		return maxSubSteps_;
	}

	void
	PhysxScene::setFixedTimeStep(float fixedTimeStep) noexcept
	{
		fixedTimeStep_ = fixedTimeStep;
	}

	float
	PhysxScene::getFixedTimeStep() noexcept
	{
		return fixedTimeStep_;
	}

	void
	PhysxScene::simulate(float time)
	{
		for (int i = 0; i < maxSubSteps_; i++)
		{
			px_scene->simulate(time / maxSubSteps_);
			px_scene->fetchResults(true);
		}
	}

	void
	PhysxScene::reset()
	{
	}

	void
	PhysxScene::fetchResults()
	{
		physx::PxU32 nbActiveActors;
		physx::PxActor** activeActors = px_scene->getActiveActors(nbActiveActors);

		for (physx::PxU32 i = 0; i < nbActiveActors; ++i)
		{
			PhysicsListener* listener = static_cast<PhysicsListener*>(activeActors[i]->userData);
			if (listener)
				listener->onFetchResult();
		}
	}
}