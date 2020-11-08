#ifndef OCTOON_PHYSICS_JOINT_H_
#define OCTOON_PHYSICS_JOINT_H_

#include <memory>
#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_type.h>

namespace octoon
{
	struct OCTOON_EXPORT SoftJointLimit
	{
		float bounciness;
		float contactDistance;
		float limit;
	};

	class OCTOON_EXPORT PhysicsJoint
	{
	public:
		PhysicsJoint() noexcept;
		virtual ~PhysicsJoint() noexcept;

		virtual void enableProjection(bool enable) noexcept = 0;
		virtual void enablePreprocessing(bool enable) noexcept = 0;
		virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) = 0;

	private:
		PhysicsJoint(const PhysicsJoint&) = delete;
		PhysicsJoint& operator=(const PhysicsJoint&) = delete;
	};
}

#endif