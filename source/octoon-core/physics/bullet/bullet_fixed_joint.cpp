#include "bullet_fixed_joint.h"
#include "bullet_rigidbody.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon::physics
{
	BulletFixedJoint::BulletFixedJoint() noexcept
		: joint(nullptr)
	{

	}

	BulletFixedJoint::~BulletFixedJoint() noexcept
	{
	}

	void
	BulletFixedJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		btTransform transform;
		transform.setIdentity();
		auto A = std::dynamic_pointer_cast<BulletRigidbody>(lhs);
		auto B = std::dynamic_pointer_cast<BulletRigidbody>(rhs);
		joint = new btFixedConstraint(*A->getPxRigidbody(), *B->getPxRigidbody(), transform, transform);
	}
}
