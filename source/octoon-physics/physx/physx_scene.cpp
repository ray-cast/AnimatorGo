#include "physx_scene.h"
#include "physx_rigidbody.h"
#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		physx::PxFilterFlags DefaultSimulationFilterShader(
			physx::PxFilterObjectAttributes attributes0,
			physx::PxFilterData filterData0,
			physx::PxFilterObjectAttributes attributes1,
			physx::PxFilterData filterData1,
			physx::PxPairFlags& pairFlags,
			const void* constantBlock,
			physx::PxU32 constantBlockSize)
		{
			PX_UNUSED(constantBlock);
			PX_UNUSED(constantBlockSize);

			// let triggers through
			if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
			{
				pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
				return physx::PxFilterFlags();
			}

			// Collision Group
			if (!physx::PxGetGroupCollisionFlag(filterData0.word0, filterData1.word0))
			{
				return physx::PxFilterFlag::eSUPPRESS;
			}

			filterData0.word2 = ~filterData0.word2;
			filterData1.word2 = ~filterData1.word2;

			if ((1 << filterData0.word0) & filterData1.word2 ||
				(1 << filterData1.word0) & filterData0.word2)
			{
				return physx::PxFilterFlag::eSUPPRESS;
			}

			pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

			return physx::PxFilterFlags();
		}

		PhysxScene::PhysxScene(PhysxContext* context, PhysicsSceneDesc desc)
			: context(nullptr)
			, px_scene(nullptr)
		{
			physx::PxSceneDesc sceneDesc(context->getPxPhysics()->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(desc.gravity.x, desc.gravity.y, desc.gravity.z);
			sceneDesc.cpuDispatcher = context->getPxDefaultCpuDispatcher();
			sceneDesc.filterShader = DefaultSimulationFilterShader;

			px_scene = context->getPxPhysics()->createScene(sceneDesc);
			px_scene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);

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

		void PhysxScene::addRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
		{
			std::shared_ptr<PhysxRigidbody> physx_rigidbody = std::dynamic_pointer_cast<PhysxRigidbody>(rigidbody);
			px_scene->addActor(*physx_rigidbody->getPxRigidbody());
		}

		void PhysxScene::simulate(float time)
		{
			px_scene->simulate(time);
		}

		void
		PhysxScene::fetchStart()
		{
			px_scene->fetchResults(true);

			physx::PxU32 nbActiveActors;
			physx::PxActor** activeActors = px_scene->getActiveActors(nbActiveActors);
			for (physx::PxU32 i = 0; i < nbActiveActors; ++i)
			{
				PhysicsListener* listener = static_cast<PhysicsListener*>(activeActors[i]->userData);
				if (listener)
					listener->onFetchResult();
			}
		}

		void
		PhysxScene::fetchFinish()
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
}

