#ifndef OCTOON_PHYSX_FIXED_JOINT_H_
#define OCTOON_PHYSX_FIXED_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_fixed_joint.h>

#include "physx_joint.h"
#include "physx_type.h"

namespace octoon
{
	class OCTOON_EXPORT PhysxFixedJoint: public PhysicsFixedJoint, public PhysxJoint
	{
	public:
		PhysxFixedJoint(PhysxContext* context) noexcept;
		virtual ~PhysxFixedJoint() noexcept;

		virtual void enablePreprocessing(bool enable) noexcept override;
		virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;

	private:
		PhysxFixedJoint(const PhysxFixedJoint&) = delete;
		PhysxFixedJoint& operator=(const PhysxFixedJoint&) = delete;

	private:
		PhysxContext* context_;
		physx::PxFixedJoint* joint_;
	};
}

#endif