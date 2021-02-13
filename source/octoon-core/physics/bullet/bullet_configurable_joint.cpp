#include "bullet_configurable_joint.h"
#include "bullet_context.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	BulletConfigurableJoint::BulletConfigurableJoint() noexcept
	{
	}

	BulletConfigurableJoint::~BulletConfigurableJoint() noexcept
	{
	}

	void
	BulletConfigurableJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		auto bodyA = std::dynamic_pointer_cast<BulletRigidbody>(lhs)->getRigidbody();
		auto bodyB = std::dynamic_pointer_cast<BulletRigidbody>(rhs)->getRigidbody();

		btTransform	 bttrTransform;
		bttrTransform.setIdentity();
		//bttrTransform.setOrigin(btVector3(_translate.x, _translate.y, _translate.z));
		//bttrTransform.setRotation(btQuaternion(_quaternion.x, _quaternion.y, _quaternion.z, _quaternion.w));

		btTransform bttrRigidAInvTransform = bodyA->getWorldTransform().inverse();
		btTransform bttrRigidBInvTransform = bodyB->getWorldTransform().inverse();

		bttrRigidAInvTransform = bttrRigidAInvTransform * bttrTransform;
		bttrRigidBInvTransform = bttrRigidBInvTransform * bttrTransform;

		_joint = std::make_unique<btGeneric6DofSpringConstraint>(*bodyA, *bodyB, bttrRigidAInvTransform, bttrRigidBInvTransform, true);

	}

	void
	BulletConfigurableJoint::setRigidATransform(const math::float3& position, const math::Quaternion& rotation) noexcept
	{
	}

	void
	BulletConfigurableJoint::setRigidBTransform(const math::float3& position, const math::Quaternion& rotation) noexcept
	{
	}

	void BulletConfigurableJoint::setXMotion(ConfigurableJointMotion motion)
	{
	}

	void BulletConfigurableJoint::setYMotion(ConfigurableJointMotion motion)
	{
	}

	void BulletConfigurableJoint::setZMotion(ConfigurableJointMotion motion)
	{
	}

	void BulletConfigurableJoint::setAngularXMotion(ConfigurableJointMotion motion)
	{		
	}

	void BulletConfigurableJoint::setAngularYMotion(ConfigurableJointMotion motion)
	{
	}

	void BulletConfigurableJoint::setAngularZMotion(ConfigurableJointMotion motion)
	{
	}

	ConfigurableJointMotion BulletConfigurableJoint::getXMotion()
	{
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion BulletConfigurableJoint::getYMotion()
	{
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion BulletConfigurableJoint::getZMotion()
	{
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion BulletConfigurableJoint::getAngularXMotion()
	{
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion BulletConfigurableJoint::getAngularYMotion()
	{
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion BulletConfigurableJoint::getAngularZMotion()
	{
			return ConfigurableJointMotion::Locked;
	}

	void
	BulletConfigurableJoint::setLowXLimit(float value)
	{
	}

	void
	BulletConfigurableJoint::setLowYLimit(float value)
	{
	}

	void
	BulletConfigurableJoint::setLowZLimit(float value)
	{
	}

	void
	BulletConfigurableJoint::setHighXLimit(float value)
	{
	}

	void
	BulletConfigurableJoint::setHighYLimit(float value)
	{
	}

	void
	BulletConfigurableJoint::setHighZLimit(float value)
	{
	}

	void
	BulletConfigurableJoint::setDistanceLimit(float distance)
	{
	}

	void
	BulletConfigurableJoint::setTwistLimit(float min, float max)
	{
	}

	void
	BulletConfigurableJoint::setSwingLimit(float limit_y, float limit_z)
	{
	}

	void
	BulletConfigurableJoint::setPyramidSwingLimit(float min_y, float max_y, float min_z, float max_z)
	{
	}

	void
	BulletConfigurableJoint::setDriveMotionX(float motion)
	{
	}

	void
	BulletConfigurableJoint::setDriveMotionY(float motion)
	{
	}

	void
	BulletConfigurableJoint::setDriveMotionZ(float motion)
	{
	}

	void 
	BulletConfigurableJoint::setDriveAngularX(float motion)
	{
	}

	void 
	BulletConfigurableJoint::setDriveAngularY(float motion)
	{
	}

	void
	BulletConfigurableJoint::setDriveAngularZ(float motion)
	{
	}

	void
	BulletConfigurableJoint::enableProjection(bool enable) noexcept
	{
	}

	void
	BulletConfigurableJoint::enablePreprocessing(bool enable) noexcept
	{
	}
}