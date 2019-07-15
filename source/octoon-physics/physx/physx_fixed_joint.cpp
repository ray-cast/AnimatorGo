#include "physx_fixed_joint.h"

#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxFixedJoint::PhysxFixedJoint(PhysxContext * context, std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) noexcept
			:joint(nullptr)
		{
			std::shared_ptr<PhysxRigidbody> px_lhs = std::dynamic_pointer_cast<PhysxRigidbody>(lhs);
			std::shared_ptr<PhysxRigidbody> px_rhs = std::dynamic_pointer_cast<PhysxRigidbody>(rhs);
			joint = physx::PxFixedJointCreate(*context->getPxPhysics(),
				px_lhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f),
				px_rhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f));
		}

		PhysxFixedJoint::~PhysxFixedJoint() noexcept
		{
		}

	}
}
