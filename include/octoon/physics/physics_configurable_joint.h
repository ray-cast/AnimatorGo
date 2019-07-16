#ifndef OCTOON_PHYSICS_CONFIGURABLE_JOINT_H_
#define OCTOON_PHYSICS_CONFIGURABLE_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_type.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysicsConfigurableJoint
		{
		public:
			PhysicsConfigurableJoint() noexcept;
			virtual ~PhysicsConfigurableJoint() noexcept;

		private:
			PhysicsConfigurableJoint(const PhysicsConfigurableJoint&) = delete;
			PhysicsConfigurableJoint& operator=(const PhysicsConfigurableJoint&) = delete;
		};
	}
}

#endif