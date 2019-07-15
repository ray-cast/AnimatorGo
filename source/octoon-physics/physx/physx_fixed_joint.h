#ifndef OCTOON_PHYSX_FIXED_JOINT_H_
#define OCTOON_PHYSX_FIXED_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_context.h>
#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxFixedJoint
		{
		public:
			PhysxFixedJoint(PhysxContext* context, std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) noexcept;
			virtual ~PhysxFixedJoint() noexcept;

		private:
			PhysxFixedJoint(const PhysxFixedJoint&) = delete;
			PhysxFixedJoint& operator=(const PhysxFixedJoint&) = delete;
		private:
			physx::PxFixedJoint* joint;
		};
	}
}

#endif