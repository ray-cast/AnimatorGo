#include "bullet_fixed_joint.h"
#include "bullet_rigidbody.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	BulletFixedJoint::BulletFixedJoint() noexcept
		: joint_(nullptr)
	{

	}

	BulletFixedJoint::~BulletFixedJoint() noexcept
	{
	}

	void
	BulletFixedJoint::enableProjection(bool enable) noexcept
	{
	}

	void
	BulletFixedJoint::enablePreprocessing(bool enable) noexcept
	{
	}

	void
	BulletFixedJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		btTransform transform;
		transform.setIdentity();
		auto A = std::dynamic_pointer_cast<BulletRigidbody>(lhs);
		auto B = std::dynamic_pointer_cast<BulletRigidbody>(rhs);
		joint_ = new btFixedConstraint(*A->getRigidbody(), *B->getRigidbody(), transform, transform);
	}

	btTypedConstraint*
	BulletFixedJoint::getConstraint() const
	{
		return joint_;
	}
}
