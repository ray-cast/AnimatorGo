#include "physx_fixed_joint.h"
#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	PhysxFixedJoint::PhysxFixedJoint(PhysxContext* context) noexcept
		: context_(context)
		, joint_(nullptr)
	{
			
	}

	PhysxFixedJoint::~PhysxFixedJoint() noexcept
	{
	}

	void
	PhysxFixedJoint::enableProjection(bool enable) noexcept
	{
		joint_->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, enable);
	}

	void
	PhysxFixedJoint::enablePreprocessing(bool enable) noexcept
	{
		joint_->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_PREPROCESSING, enable);
	}

	void
	PhysxFixedJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		std::shared_ptr<PhysxRigidbody> px_lhs = std::dynamic_pointer_cast<PhysxRigidbody>(lhs);
		std::shared_ptr<PhysxRigidbody> px_rhs = std::dynamic_pointer_cast<PhysxRigidbody>(rhs);
		joint_ = physx::PxFixedJointCreate(*context_->getPxPhysics(),
			px_lhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f),
			px_rhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f));
	}
}
