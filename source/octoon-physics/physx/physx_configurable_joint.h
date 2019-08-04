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

			virtual void setRigidATransform(const math::float3& position, const math::Quaternion& rotation) noexcept;
			virtual void setRigidBTransform(const math::float3& position, const math::Quaternion& rotation) noexcept;

			virtual void setXMotion(ConfigurableJointMotion motion) override;
			virtual void setYMotion(ConfigurableJointMotion motion) override;
			virtual void setZMotion(ConfigurableJointMotion motion) override;

			virtual void setAngularXMotion(ConfigurableJointMotion motion) override;
			virtual void setAngularYMotion(ConfigurableJointMotion motion) override;
			virtual void setAngularZMotion(ConfigurableJointMotion motion) override;

			virtual ConfigurableJointMotion getXMotion() override;
			virtual ConfigurableJointMotion getYMotion() override;
			virtual ConfigurableJointMotion getZMotion() override;

			virtual ConfigurableJointMotion getAngularXMotion() override;
			virtual ConfigurableJointMotion getAngularYMotion() override;
			virtual ConfigurableJointMotion getAngularZMotion() override;

			virtual void setLowXLimit(float limit);
			virtual void setLowYLimit(float limit);
			virtual void setLowZLimit(float limit);

			virtual void setHighXLimit(float limit);
			virtual void setHighYLimit(float limit);
			virtual void setHighZLimit(float limit);

			virtual void setLowAngularXLimit(float limit);
			virtual void setHighAngularXLimit(float limit);

			virtual void setSwingLimit(float limit_y, float limit_z);
			virtual void setPyramidSwingLimit(float min_y, float max_y, float min_z, float max_z);

			virtual void setDriveMotionX(float motion);
			virtual void setDriveMotionY(float motion);
			virtual void setDriveMotionZ(float motion);

			virtual void setDriveAngularX(float motion);
			virtual void setDriveAngularY(float motion);
			virtual void setDriveAngularZ(float motion);


		private:
			PhysxConfigurableJoint(const PhysxConfigurableJoint&) = delete;
			PhysxConfigurableJoint& operator=(const PhysxConfigurableJoint&) = delete;

		private:
			PhysxContext* context_;
			physx::PxD6Joint* joint_;
		};
	}
}

#endif