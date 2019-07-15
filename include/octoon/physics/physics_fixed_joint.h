#ifndef OCTOON_PHYSICS_FIXED_JOINT_H_
#define OCTOON_PHYSICS_FIXED_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_type.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysicsFixedJoint
		{
		public:
			PhysicsFixedJoint() noexcept;
			virtual ~PhysicsFixedJoint() noexcept;

		private:
			PhysicsFixedJoint(const PhysicsFixedJoint&) = delete;
			PhysicsFixedJoint& operator=(const PhysicsFixedJoint&) = delete;
		};
	}
}

#endif