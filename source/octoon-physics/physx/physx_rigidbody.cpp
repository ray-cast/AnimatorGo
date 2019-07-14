#include "physx_rigidbody.h"
#include "physx_context.h"

#include<octoon/runtime/except.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxRigidbody::PhysxRigidbody(PhysxContext * context, PhysicsRigidbodyDesc desc)
			:context(nullptr), px_rigidbody(nullptr)
		{
			physx::PxTransform pose;
			pose.p = physx::PxVec3(desc.translate.x, desc.translate.y, desc.translate.z);
			pose.q = physx::PxQuat(desc.rotation.x, desc.rotation.y, desc.rotation.z, desc.rotation.w);

			if (desc.type == PhysicsRigidbodyType::Static)
			{
				auto rigidbody = context->getPxPhysics()->createRigidStatic(pose);
				if (!rigidbody)
					throw  runtime::runtime_error::create("create body failed!");
				px_rigidbody = rigidbody;
			}
			else if(desc.type == PhysicsRigidbodyType::Dynamic)
			{
				auto rigidbody = context->getPxPhysics()->createRigidDynamic(pose);
				if (!rigidbody)
					throw  runtime::runtime_error::create("create body failed!");

				rigidbody->setMass(desc.mass);

				px_rigidbody = rigidbody;
			}
		}

		PhysxRigidbody::~PhysxRigidbody()
		{
		}

		physx::PxRigidActor * PhysxRigidbody::getPxRigidbody()
		{
			return px_rigidbody;
		}
	}
}


