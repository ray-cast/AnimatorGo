#ifndef OCTOON_PHYSX_CONFIGURABLE_JOINT_H_
#define OCTOON_PHYSX_CONFIGURABLE_JOINT_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_configurable_joint.h>

#include "physx_joint.h"
#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxConfigurableJoint : public PhysicsConfigurableJoint, public PhysxJoint
		{
		public:
			PhysxConfigurableJoint(PhysxContext* context) noexcept;
			virtual ~PhysxConfigurableJoint() noexcept;

			virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;

			virtual void setAngularXMotion(ConfigurableJointMotion motion) override;
			virtual ConfigurableJointMotion getAngularXMotion() override;

			virtual void setAngularYMotion(ConfigurableJointMotion motion) override;
			virtual ConfigurableJointMotion getAngularYMotion() override;

			virtual void setAngularZMotion(ConfigurableJointMotion motion) override;
			virtual ConfigurableJointMotion getAngularZMotion() override;

			virtual void setXMotion(ConfigurableJointMotion motion) override;
			virtual ConfigurableJointMotion getXMotion() override;

			virtual void setYMotion(ConfigurableJointMotion motion) override;
			virtual ConfigurableJointMotion getYMotion() override;

			virtual void setZMotion(ConfigurableJointMotion motion) override;
			virtual ConfigurableJointMotion getZMotion() override;

		private:
			PhysxConfigurableJoint(const PhysxConfigurableJoint&) = delete;
			PhysxConfigurableJoint& operator=(const PhysxConfigurableJoint&) = delete;
		private:
			PhysxContext* context;
			physx::PxD6Joint* joint;
		};
	}
}

#endif