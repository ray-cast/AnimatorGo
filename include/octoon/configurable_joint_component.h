#ifndef OCTOON_CONFIGURALE_JOINT_COMPONENT_H_
#define OCTOON_CONFIGURALE_JOINT_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/math/math.h>

#include <octoon/rigidbody_component.h>
#include <octoon/physics/physics_joint.h>
#include <octoon/physics/physics_configurable_joint.h>

namespace octoon
{
	class OCTOON_EXPORT ConfigurableJointComponent final : public JointComponent
	{
		OctoonDeclareSubInterface(ConfigurableJointComponent, GameComponent)
	public:
		ConfigurableJointComponent() noexcept;
		virtual ~ConfigurableJointComponent() noexcept;

		void setTargetPosition(const math::float3& position) noexcept;
		void setTargetVelocity(const math::float3& velocity) noexcept;
		void setTargetRotation(const math::Quaternion& quat) noexcept;

		const math::float3& getTargetPosition() const noexcept;
		const math::float3& setTargetVelocity() const noexcept;
		const math::Quaternion& getTargetRotation() const noexcept;

		void setXMotion(ConfigurableJointMotion motion) noexcept;
		void setYMotion(ConfigurableJointMotion motion) noexcept;
		void setZMotion(ConfigurableJointMotion motion) noexcept;

		void setAngularXMotion(ConfigurableJointMotion motion) noexcept;
		void setAngularYMotion(ConfigurableJointMotion motion) noexcept;
		void setAngularZMotion(ConfigurableJointMotion motion) noexcept;

		ConfigurableJointMotion getXMotion() const noexcept;
		ConfigurableJointMotion getYMotion() const noexcept;
		ConfigurableJointMotion getZMotion() const noexcept;

		ConfigurableJointMotion getAngularXMotion() const noexcept;
		ConfigurableJointMotion getAngularYMotion() const noexcept;
		ConfigurableJointMotion getAngularZMotion() const noexcept;

		void setLowXLimit(float limit) noexcept;
		void setLowYLimit(float limit) noexcept;
		void setLowZLimit(float limit) noexcept;

		void setHighXLimit(float limit) noexcept;
		void setHighYLimit(float limit) noexcept;
		void setHighZLimit(float limit) noexcept;

		void setDistanceLimit(float distance) noexcept;
		void setTwistLimit(float min, float max) noexcept;
		void setSwingLimit(float y, float z) noexcept;
		void setAngularLimit(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) noexcept;
		void setPyramidSwingLimit(float minY, float maxY, float minZ, float maxZ) noexcept;

		void setDriveMotionX(float motion) noexcept;
		void setDriveMotionY(float motion) noexcept;
		void setDriveMotionZ(float motion) noexcept;

		void setDriveAngularX(float motion) noexcept;
		void setDriveAngularY(float motion) noexcept;
		void setDriveAngularZ(float motion) noexcept;

		float getLowXLimit() const noexcept;
		float getLowYLimit() const noexcept;
		float getLowZLimit() const noexcept;

		float getHighXLimit() const noexcept;
		float getHighYLimit() const noexcept;
		float getHighZLimit() const noexcept;

		float getDriveMotionX() const noexcept;
		float getDriveMotionY() const noexcept;
		float getDriveMotionZ() const noexcept;

		float getDriveAngularX() const noexcept;
		float getDriveAngularY() const noexcept;
		float getDriveAngularZ() const noexcept;

		void getAngularLimit(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) noexcept;

		void enableProjection(bool enable) noexcept;
		void enablePreprocessing(bool enable) noexcept;

		std::shared_ptr<RigidbodyComponent> getTarget() noexcept;
		void setTarget(std::shared_ptr<RigidbodyComponent> component) noexcept;

		GameComponentPtr clone() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onAttachComponent(const GameComponentPtr& component) noexcept override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

	protected:
		void setupConfigurableJoint();
		void setupConfigurableTransform(const math::float3& position, const math::Quaternion& rotation);

	private:
		bool enableProjection_;
		bool enablePreprocessing_;

		float lowX_;
		float lowY_;
		float lowZ_;

		float highX_;
		float highY_;
		float highZ_;

		float distanceLimit_;

		float lowAngleXLimit_;
		float lowAngleYLimit_;
		float lowAngleZLimit_;

		float highAngleXLimit_;
		float highAngleYLimit_;
		float highAngleZLimit_;

		math::float3 targetPosition_;
		math::float3 targetVelocity_;
		math::Quaternion targetRotation_;

		math::float3 driveMotion_;
		math::float3 driveAngular_;

		ConfigurableJointMotion motionX_;
		ConfigurableJointMotion motionY_;
		ConfigurableJointMotion motionZ_;
		ConfigurableJointMotion angularMotionX_;
		ConfigurableJointMotion angularMotionY_;
		ConfigurableJointMotion angularMotionZ_;

		std::shared_ptr<PhysicsConfigurableJoint> joint_;
		std::weak_ptr<RigidbodyComponent> another_;
	};
}

#endif