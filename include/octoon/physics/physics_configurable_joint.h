#ifndef OCTOON_PHYSICS_CONFIGURABLE_JOINT_H_
#define OCTOON_PHYSICS_CONFIGURABLE_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_joint.h>

#include <octoon/physics/physics_type.h>

namespace octoon
{
	namespace physics
	{

		enum class ConfigurableJointMotion
		{
			Locked,
			Limited,
			Free
		};

		class OCTOON_EXPORT PhysicsConfigurableJoint: public PhysicsJoint
		{
		public:
			PhysicsConfigurableJoint() noexcept;
			virtual ~PhysicsConfigurableJoint() noexcept;

			virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) = 0;

			virtual void setAngularXMotion(ConfigurableJointMotion motion) = 0;
			virtual ConfigurableJointMotion getAngularXMotion() = 0;

			virtual void setAngularYMotion(ConfigurableJointMotion motion) = 0;
			virtual ConfigurableJointMotion getAngularYMotion() = 0;

			virtual void setAngularZMotion(ConfigurableJointMotion motion) = 0;
			virtual ConfigurableJointMotion getAngularZMotion() = 0;

			virtual void setXMotion(ConfigurableJointMotion motion) = 0;
			virtual ConfigurableJointMotion getXMotion() = 0;

			virtual void setYMotion(ConfigurableJointMotion motion) = 0;
			virtual ConfigurableJointMotion getYMotion() = 0;

			virtual void setZMotion(ConfigurableJointMotion motion) = 0;
			virtual ConfigurableJointMotion getZMotion() = 0;

		private:
			PhysicsConfigurableJoint(const PhysicsConfigurableJoint&) = delete;
			PhysicsConfigurableJoint& operator=(const PhysicsConfigurableJoint&) = delete;
		};
	}
}

#endif