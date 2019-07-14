#include "physx_scene.h"

#include "physx_rigidbody.h"
#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxScene::PhysxScene(PhysxContext * context, PhysicsSceneDesc desc)
			:context(nullptr), px_scene(nullptr)
		{
			physx::PxSceneDesc sceneDesc(context->getPxPhysics()->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(desc.gravity.x, desc.gravity.y, desc.gravity.z);
			sceneDesc.cpuDispatcher = context->getPxDefaultCpuDispatcher();
			sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
			px_scene = context->getPxPhysics()->createScene(sceneDesc);
		}

		PhysxScene::~PhysxScene()
		{
		}

		void PhysxScene::addRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
		{
			std::shared_ptr<PhysxRigidbody> physx_rigidbody = std::dynamic_pointer_cast<PhysxRigidbody>(rigidbody);
			px_scene->addActor(*physx_rigidbody->getPxRigidbody());
		}
	}
}

