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
		this->frameA_ = std::dynamic_pointer_cast<BulletRigidbody>(lhs);
		this->frameB_ = std::dynamic_pointer_cast<BulletRigidbody>(rhs);

		auto bodyA = frameA_->getRigidbody();
		auto bodyB = frameB_->getRigidbody();

		transformA_ = bodyA->getWorldTransform().inverse();
		transformB_ = bodyB->getWorldTransform().inverse();

		_joint = std::make_unique<btGeneric6DofSpringConstraint>(*bodyA, *bodyB, transformA_, transformB_, true);
	}

	void
	BulletConfigurableJoint::setFrames(const math::float3& positionA, const math::Quaternion& rotationA, const math::float3& positionB, const math::Quaternion& rotationB)
	{
		transformA_.setIdentity();
		transformA_.setOrigin(btVector3(positionA.x, positionA.y, positionA.z));
		transformA_.setRotation(btQuaternion(rotationA.x, rotationA.y, rotationA.z, rotationA.w));

		transformB_.setIdentity();
		transformB_.setOrigin(btVector3(positionB.x, positionB.y, positionB.z));
		transformB_.setRotation(btQuaternion(rotationB.x, rotationB.y, rotationB.z, rotationB.w));

		_joint->setFrames(transformA_, transformB_);
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
			_joint->setLimit(0, 1, -1);
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
			_joint->setLimit(1, 1, -1);
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
			_joint->setLimit(2, 1, -1);
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
			_joint->setLimit(3, 1, -1);
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
			_joint->setLimit(4, 1, -1);
	}

	void
	BulletConfigurableJoint::setAngularZMotion(ConfigurableJointMotion motion)
	{
		btVector3 lowerLimit, upperLimit;
		_joint->getAngularLowerLimit(lowerLimit);
		_joint->getAngularUpperLimit(upperLimit);

		if (motion == ConfigurableJointMotion::Locked)
			_joint->setLimit(5, 0, 0);
		else if (motion == ConfigurableJointMotion::Limited)
			_joint->setLimit(5, lowerLimit.z(), upperLimit.z());
		else if (motion == ConfigurableJointMotion::Free)
			_joint->setLimit(5, 1, -1);
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getXMotion()
	{
		auto limit = _joint->getTranslationalLimitMotor();

		if (limit->m_lowerLimit.x() == limit->m_upperLimit.x())
			return ConfigurableJointMotion::Locked;
		else if (limit->m_lowerLimit.x() < limit->m_upperLimit.x())
			return ConfigurableJointMotion::Limited;
		else
			return ConfigurableJointMotion::Free;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getYMotion()
	{
		auto limit = _joint->getTranslationalLimitMotor();

		if (limit->m_lowerLimit.y() == limit->m_upperLimit.y())
			return ConfigurableJointMotion::Locked;
		else if (limit->m_lowerLimit.y() < limit->m_upperLimit.y())
			return ConfigurableJointMotion::Limited;
		else
			return ConfigurableJointMotion::Free;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getZMotion()
	{
		auto limit = _joint->getTranslationalLimitMotor();

		if (limit->m_lowerLimit.z() == limit->m_upperLimit.z())
			return ConfigurableJointMotion::Locked;
		else if (limit->m_lowerLimit.z() < limit->m_upperLimit.z())
			return ConfigurableJointMotion::Limited;
		else
			return ConfigurableJointMotion::Free;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getAngularXMotion()
	{
		btVector3 lowerLimit, upperLimit;
		_joint->getAngularLowerLimit(lowerLimit);
		_joint->getAngularUpperLimit(upperLimit);

		if (lowerLimit.x() == upperLimit.x())
			return ConfigurableJointMotion::Locked;
		else if (lowerLimit.x() < upperLimit.x())
			return ConfigurableJointMotion::Limited;
		else
			return ConfigurableJointMotion::Free;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getAngularYMotion()
	{
		btVector3 lowerLimit, upperLimit;
		_joint->getAngularLowerLimit(lowerLimit);
		_joint->getAngularUpperLimit(upperLimit);

		if (lowerLimit.y() == upperLimit.y())
			return ConfigurableJointMotion::Locked;
		else if (lowerLimit.y() < upperLimit.y())
			return ConfigurableJointMotion::Limited;
		else
			return ConfigurableJointMotion::Free;
	}

	ConfigurableJointMotion
	BulletConfigurableJoint::getAngularZMotion()
	{
		btVector3 lowerLimit, upperLimit;
		_joint->getAngularLowerLimit(lowerLimit);
		_joint->getAngularUpperLimit(upperLimit);

		if (lowerLimit.z() == upperLimit.z())
			return ConfigurableJointMotion::Locked;
		else if (lowerLimit.z() < upperLimit.z())
			return ConfigurableJointMotion::Limited;
		else
			return ConfigurableJointMotion::Free;
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
		btVector3 lowerLimit, upperLimit;
		_joint->getAngularLowerLimit(lowerLimit);
		_joint->getAngularUpperLimit(upperLimit);

		_joint->setAngularLowerLimit(btVector3(min, lowerLimit.y(), lowerLimit.z()));
		_joint->setAngularUpperLimit(btVector3(max, upperLimit.y(), upperLimit.z()));
	}

	void
	BulletConfigurableJoint::setSwingLimit(float limit_y, float limit_z)
	{
		btVector3 lowerLimit, upperLimit;
		_joint->getAngularLowerLimit(lowerLimit);
		_joint->getAngularUpperLimit(upperLimit);

		_joint->setAngularLowerLimit(btVector3(lowerLimit.x(), -limit_y, -limit_z));
		_joint->setAngularUpperLimit(btVector3(upperLimit.y(), limit_y, limit_z));
	}

	void
	BulletConfigurableJoint::setPyramidSwingLimit(float min_y, float max_y, float min_z, float max_z)
	{
		btVector3 lowerLimit, upperLimit;
		_joint->getAngularLowerLimit(lowerLimit);
		_joint->getAngularUpperLimit(upperLimit);

		_joint->setAngularLowerLimit(btVector3(lowerLimit.x(), min_y, min_z));
		_joint->setAngularUpperLimit(btVector3(upperLimit.y(), max_y, max_z));
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

	btTypedConstraint*
	BulletConfigurableJoint::getConstraint() const
	{
		return _joint.get();
	}
}