#ifndef OCTOON_PHYSX_JOINT_H_
#define OCTOON_PHYSX_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_joint.h>
#include <octoon/physics/physics_type.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxJoint :public PhysicsJoint
		{
		public:
			PhysxJoint() noexcept;
			virtual ~PhysxJoint() noexcept;

			virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) = 0;

		private:
			PhysxJoint(const PhysxJoint&) = delete;
			PhysxJoint& operator=(const PhysxJoint&) = delete;
		};
	}
}

#endif