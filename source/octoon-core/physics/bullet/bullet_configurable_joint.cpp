#include "bullet_configurable_joint.h"
#include "bullet_context.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	BulletConfigurableJoint::BulletConfigurableJoint() noexcept
		: translate_(math::float3::Zero)
		, quaternion_(math::Quaternion::Zero)
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

		transform_.setIdentity();
		transform_.setOrigin(btVector3(translate_.x, translate_.y, translate_.z));
		transform_.setRotation(btQuaternion(quaternion_.x, quaternion_.y, quaternion_.z, quaternion_.w));

		transformA_ = bodyA->getWorldTransform().inverse() * transform_;
		transformB_ = bodyB->getWorldTransform().inverse() * transform_;

		_joint = std::make_unique<btGeneric6DofSpringConstraint>(*bodyA, *bodyB, transformA_, transformB_, true);
	}

	void
	BulletConfigurableJoint::setRigidATransform(const math::float3& position, const math::Quaternion& rotation) noexcept
	{
		transformA_.setIdentity();
		transformA_.setOrigin(btVector3(position.x, position.y, position.z));
		transformA_.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
		transformA_ *= transform_;

		_joint->calculateTransforms(transformA_, transformB_);
	}

	void
	BulletConfigurableJoint::setRigidBTransform(const math::float3& position, const math::Quaternion& rotation) noexcept
	{
		transformB_.setIdentity();
		transformB_.setOrigin(btVector3(position.x, position.y, position.z));
		transformB_.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
		transformB_ *= transform_;

		_joint->calculateTransforms(transformA_, transformB_);
	}

	void
	BulletConfigurableJoint::setXMotion(ConfigurableJointMotion motion)
	{
		auto limit = _joint->getTranslationalLimitMotor();
		
		if (motion == ConfigurableJointMotion::Locked)
			_joint->setLimit(0, 0, 0);
		else if (motion == ConfigurableJointMotion::Limited)
			_joint->setLimit(0, limit->m_lowerLimit.x(), limit->m_upperLimit.x());
		else if (motion == ConfigurableJointMotion::Free)
			_joint->setLimit(0, std::numeric_limits<btScalar>::max(), std::numeric_limits<btScalar>::lowest());
	}

	void
	BulletConfigurableJoint::setYMotion(ConfigurableJointMotion motion)
	{
		auto limit = _joint->getTranslationalLimitMotor();

		if (motion == ConfigurableJointMotion::Locked)
			_joint->setLimit(1, 0, 0);
		else if (motion == ConfigurableJointMotion::Limited)
			_joint->setLimit(1, limit->m_lowerLimit.y(), limit->m_upperLimit.y());
		else if (motion == ConfigurableJointMotion::Free)
			_joint->setLimit(1, std::numeric_limits<btScalar>::max(), std::numeric_limits<btScalar>::lowest());
	}

	void
	BulletConfigurableJoint::setZMotion(ConfigurableJointMotion motion)
	{
		auto limit = _joint->getTranslationalLimitMotor();

		if (motion == ConfigurableJointMotion::Locked)
			_joint->setLimit(2, 0, 0);
		else if (motion == ConfigurableJointMotion::Limited)
			_joint->setLimit(2, limit->m_lowerLimit.z(), limit->m_upperLimit.z());
		else if (motion == ConfigurableJointMotion::Free)
			_joint->setLimit(2, std::numeric_limits<btScalar>::max(), std::numeric_limits<btScalar>::lowest());
	}

	void
	BulletConfigurableJoint::setAngularXMotion(ConfigurableJointMotion motion)
	{
		btVector3 lowerLimit;
		_joint->getAngularLowerLimit(lowerLimit);

		btVector3 upperLimit;
		_joint->getAngularUpperLimit(upperLimit);

		if (motion == ConfigurableJointMotion::Locked)
			_joint->setLimit(3, 0, 0);
		else if (motion == ConfigurableJointMotion::Limited)
			_joint->setLimit(3, lowerLimit.x(), upperLimit.x());
		else if (motion == ConfigurableJointMotion::Free)
			_joint->setLimit(3, std::numeric_limits<btScalar>::max(), std::numeric_limits<btScalar>::lowest());
	}

	void
	BulletConfigurableJoint::setAngularYMotion(ConfigurableJointMotion motion)
	{
		btVector3 lowerLimit;
		_joint->getAngularLowerLimit(lowerLimit);

		btVector3 upperLimit;
		_joint->getAngularUpperLimit(upperLimit);

		if (motion == ConfigurableJointMotion::Locked)
			_joint->setLimit(4, 0, 0);
		else if (motion == ConfigurableJointMotion::Limited)
			_joint->setLimit(4, lowerLimit.y(), upperLimit.y());
		else if (motion == ConfigurableJointMotion::Free)
			_joint->setLimit(4, std::numeric_limits<btScalar>::max(), std::numeric_limits<btScalar>::lowest());
	}

	void
	BulletConfigurableJoint::setAngularZMotion(ConfigurableJointMotion motion)
	{
		btVector3 lowerLimit;
		_joint->getAngularLowerLimit(lowerLimit);

		btVector3 upperLimit;
		_joint->getAngularUpperLimit(upperLimit);

		if (motion == ConfigurableJointMotion::Locked)
			_joint->setLimit(5, 0, 0);
		else if (motion == ConfigurableJointMotion::Limited)
			_joint->setLimit(5, lowerLimit.z(), upperLimit.z());
		else if (motion == ConfigurableJointMotion::Free)
			_joint->setLimit(5, std::numeric_limits<btScalar>::max(), std::numeric_limits<btScalar>::lowest());
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getXMotion()
	{
		return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getYMotion()
	{
		return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getZMotion()
	{
		return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getAngularXMotion()
	{
		return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getAngularYMotion()
	{
		return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getAngularZMotion()
	{
		return ConfigurableJointMotion::Locked;
	}

	void
	BulletConfigurableJoint::setLowXLimit(float value)
	{
		btVector3 lowerLimit;
		_joint->getLinearLowerLimit(lowerLimit);
		_joint->setLinearLowerLimit(btVector3(value, lowerLimit.y(), lowerLimit.z()));
	}

	void
	BulletConfigurableJoint::setLowYLimit(float value)
	{
		btVector3 lowerLimit;
		_joint->getLinearLowerLimit(lowerLimit);
		_joint->setLinearLowerLimit(btVector3(lowerLimit.x(), value, lowerLimit.z()));
	}

	void
	BulletConfigurableJoint::setLowZLimit(float value)
	{
		btVector3 lowerLimit;
		_joint->getLinearLowerLimit(lowerLimit);
		_joint->setLinearLowerLimit(btVector3(lowerLimit.x(), lowerLimit.y(), value));
	}

	void
	BulletConfigurableJoint::setHighXLimit(float value)
	{
		btVector3 upperLimit;
		_joint->getLinearUpperLimit(upperLimit);
		_joint->setLinearUpperLimit(btVector3(value, upperLimit.y(), upperLimit.z()));
	}

	void
	BulletConfigurableJoint::setHighYLimit(float value)
	{
		btVector3 upperLimit;
		_joint->getLinearUpperLimit(upperLimit);
		_joint->setLinearUpperLimit(btVector3(upperLimit.x(), value, upperLimit.z()));
	}

	void
	BulletConfigurableJoint::setHighZLimit(float value)
	{
		btVector3 upperLimit;
		_joint->getLinearUpperLimit(upperLimit);
		_joint->setLinearUpperLimit(btVector3(upperLimit.x(), upperLimit.y(), value));
	}

	void
	BulletConfigurableJoint::setDistanceLimit(float distance)
	{
		_joint->setLinearLowerLimit(btVector3(0, 0, 0));
		_joint->setLinearUpperLimit(btVector3(distance, distance, distance));
	}

	void
	BulletConfigurableJoint::setTwistLimit(float min, float max)
	{
		_joint->setAngularLowerLimit(btVector3(min, min, min));
		_joint->setAngularUpperLimit(btVector3(max, max, max));
	}

	void
	BulletConfigurableJoint::setSwingLimit(float limit_y, float limit_z)
	{
		_joint->setAngularLowerLimit(btVector3(0, -limit_y, -limit_z));
		_joint->setAngularUpperLimit(btVector3(0, limit_y, limit_z));
	}

	void
	BulletConfigurableJoint::setPyramidSwingLimit(float min_y, float max_y, float min_z, float max_z)
	{
		_joint->setAngularLowerLimit(btVector3(0, min_y, min_z));
		_joint->setAngularUpperLimit(btVector3(0, max_y, max_z));
	}

	void
	BulletConfigurableJoint::setAngularLimit(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z)
	{
		_joint->setAngularLowerLimit(btVector3(min_x, min_y, min_z));
		_joint->setAngularUpperLimit(btVector3(min_y, max_y, max_z));
	}

	void
	BulletConfigurableJoint::setDriveMotionX(float motion)
	{
		if (motion != 0.0f)
		{
			_joint->enableSpring(0, true);
			_joint->setStiffness(0, motion);
		}
		else
		{
			_joint->enableSpring(0, false);
		}
	}

	void
	BulletConfigurableJoint::setDriveMotionY(float motion)
	{
		if (motion != 0.0f)
		{
			_joint->enableSpring(1, true);
			_joint->setStiffness(1, motion);
		}
		else
		{
			_joint->enableSpring(1, false);
		}
	}

	void
	BulletConfigurableJoint::setDriveMotionZ(float motion)
	{
		if (motion != 0.0f)
		{
			_joint->enableSpring(2, true);
			_joint->setStiffness(2, motion);
		}
		else
		{
			_joint->enableSpring(2, false);
		}
	}

	void 
	BulletConfigurableJoint::setDriveAngularX(float motion)
	{
		if (motion != 0.0f)
		{
			_joint->enableSpring(3, true);
			_joint->setStiffness(3, motion);
		}
		else
		{
			_joint->enableSpring(3, false);
		}
	}

	void 
	BulletConfigurableJoint::setDriveAngularY(float motion)
	{
		if (motion != 0.0f)
		{
			_joint->enableSpring(4, true);
			_joint->setStiffness(4, motion);
		}
		else
		{
			_joint->enableSpring(4, false);
		}
	}

	void
	BulletConfigurableJoint::setDriveAngularZ(float motion)
	{
		if (motion != 0.0f)
		{
			_joint->enableSpring(5, true);
			_joint->setStiffness(5, motion);
		}
		else
		{
			_joint->enableSpring(5, false);
		}
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