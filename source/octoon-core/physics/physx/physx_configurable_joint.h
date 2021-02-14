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
	class OCTOON_EXPORT PhysxConfigurableJoint : public PhysicsConfigurableJoint, public PhysxJoint
	{
	public:
		PhysxConfigurableJoint(PhysxContext* context) noexcept;
		virtual ~PhysxConfigurableJoint() noexcept;

		virtual void connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;

		virtual void setFrames(const math::float3& positionA, const math::Quaternion& rotationA, const math::float3& positionB, const math::Quaternion& rotationB) override;

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

		virtual void setLowXLimit(float limit) override;
		virtual void setLowYLimit(float limit) override;
		virtual void setLowZLimit(float limit) override;

		virtual void setHighXLimit(float limit) override;
		virtual void setHighYLimit(float limit) override;
		virtual void setHighZLimit(float limit) override;

		virtual void setDistanceLimit(float distance) override;
		virtual void setTwistLimit(float min, float max) override;
		virtual void setSwingLimit(float limit_y, float limit_z) override;
		virtual void setPyramidSwingLimit(float min_y, float max_y, float min_z, float max_z) override;

		virtual void setDriveMotionX(float motion) override;
		virtual void setDriveMotionY(float motion) override;
		virtual void setDriveMotionZ(float motion) override;

		virtual void setDriveAngularX(float motion) override;
		virtual void setDriveAngularY(float motion) override;
		virtual void setDriveAngularZ(float motion) override;

		virtual void enableProjection(bool enable) noexcept override;
		virtual void enablePreprocessing(bool enable) noexcept override;

	private:
		PhysxConfigurableJoint(const PhysxConfigurableJoint&) = delete;
		PhysxConfigurableJoint& operator=(const PhysxConfigurableJoint&) = delete;

	private:
		PhysxContext* context_;
		physx::PxD6Joint* joint_;
	};
}

#endif