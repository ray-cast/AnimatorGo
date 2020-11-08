#ifndef OCTOON_BULLET_FIXED_JOINT_H_
#define OCTOON_BULLET_FIXED_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_fixed_joint.h>

#include "bullet_joint.h"
#include "bullet_type.h"

namespace octoon
{
	class OCTOON_EXPORT BulletFixedJoint final : public PhysicsFixedJoint, public BulletJoint
	{
	public:
		BulletFixedJoint() noexcept;
		virtual ~BulletFixedJoint() noexcept;

		virtual void enableProjection(bool enable) noexcept override;
		virtual void enablePreprocessing(bool enable) noexcept override;
		virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;

	private:
		BulletFixedJoint(const BulletFixedJoint&) = delete;
		BulletFixedJoint& operator=(const BulletFixedJoint&) = delete;

	private:
		btFixedConstraint* joint;
	};
}

#endif