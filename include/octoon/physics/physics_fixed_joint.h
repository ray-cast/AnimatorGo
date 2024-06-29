#ifndef OCTOON_PHYSICS_FIXED_JOINT_H_
#define OCTOON_PHYSICS_FIXED_JOINT_H_

#include <memory>
#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_type.h>

namespace octoon
{
	class OCTOON_EXPORT PhysicsFixedJoint
	{
	public:
		PhysicsFixedJoint() noexcept;
		virtual ~PhysicsFixedJoint() noexcept;

		virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) = 0;

	private:
		PhysicsFixedJoint(const PhysicsFixedJoint&) = delete;
		PhysicsFixedJoint& operator=(const PhysicsFixedJoint&) = delete;
	};
}

#endif