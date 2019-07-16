#include "physx_configurable_joint.h"

#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxConfigurableJoint::PhysxConfigurableJoint(PhysxContext * context) noexcept
			:context(nullptr), joint(nullptr)
		{
		}

		PhysxConfigurableJoint::~PhysxConfigurableJoint() noexcept
		{
			if (joint)
			{
				joint->release();
				joint = nullptr;
			}
		}

		void PhysxConfigurableJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
		{
			std::shared_ptr<PhysxRigidbody> px_lhs = std::dynamic_pointer_cast<PhysxRigidbody>(lhs);
			std::shared_ptr<PhysxRigidbody> px_rhs = std::dynamic_pointer_cast<PhysxRigidbody>(rhs);
			joint = physx::PxD6JointCreate(*context->getPxPhysics(),
				px_lhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f),
				px_rhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f));
		}

	}
}
