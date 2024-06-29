#include "physx_configurable_joint.h"
#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	PhysxConfigurableJoint::PhysxConfigurableJoint(PhysxContext* context) noexcept
		: joint_(nullptr)
		, context_(context)
	{
	}

	PhysxConfigurableJoint::~PhysxConfigurableJoint() noexcept
	{
		if (joint_)
		{
			joint_->release();
			joint_ = nullptr;
		}
	}

	void
	PhysxConfigurableJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		std::shared_ptr<PhysxRigidbody> px_lhs = std::dynamic_pointer_cast<PhysxRigidbody>(lhs);
		std::shared_ptr<PhysxRigidbody> px_rhs = std::dynamic_pointer_cast<PhysxRigidbody>(rhs);

		if (joint_)
		{
			joint_->setActors(px_lhs->getPxRigidbody(), px_rhs->getPxRigidbody());
		}
		else
		{
			auto idensity = physx::PxTransform(0.f, 0.f, 0.f);
			joint_ = physx::PxD6JointCreate(*context_->getPxPhysics(), px_lhs->getPxRigidbody(), idensity, px_rhs->getPxRigidbody(), idensity);
			joint_->setConstraintFlag(physx::PxConstraintFlag::eIMPROVED_SLERP, true);			
			joint_->setProjectionLinearTolerance(0.01f);
		}
	}

	void
	PhysxConfigurableJoint::setFrames(const math::float3& positionA, const math::Quaternion& rotationA, const math::float3& positionB, const math::Quaternion& rotationB)
	{
		physx::PxTransform pxGlobalPoseA;
		pxGlobalPoseA.p = physx::PxVec3(positionA.x, positionA.y, positionA.z);
		pxGlobalPoseA.q = physx::PxQuat(rotationA.x, rotationA.y, rotationA.z, rotationA.w);

		physx::PxTransform pxGlobalPoseB;
		pxGlobalPoseB.p = physx::PxVec3(positionB.x, positionB.y, positionB.z);
		pxGlobalPoseB.q = physx::PxQuat(rotationB.x, rotationB.y, rotationB.z, rotationB.w);

		joint_->setLocalPose(physx::PxJointActorIndex::eACTOR0, pxGlobalPoseA);
		joint_->setLocalPose(physx::PxJointActorIndex::eACTOR1, pxGlobalPoseB);
	}

	void PhysxConfigurableJoint::setXMotion(ConfigurableJointMotion motion)
	{
		if (motion == ConfigurableJointMotion::Locked)
			joint_->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
		else if (motion == ConfigurableJointMotion::Limited)
			joint_->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLIMITED);
		else if (motion == ConfigurableJointMotion::Free)
			joint_->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
		else
			joint_->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
	}

	void PhysxConfigurableJoint::setYMotion(ConfigurableJointMotion motion)
	{
		if (motion == ConfigurableJointMotion::Locked)
			joint_->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
		else if (motion == ConfigurableJointMotion::Limited)
			joint_->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLIMITED);
		else if (motion == ConfigurableJointMotion::Free)
			joint_->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
		else
			joint_->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
	}

	void PhysxConfigurableJoint::setZMotion(ConfigurableJointMotion motion)
	{
		if (motion == ConfigurableJointMotion::Locked)
			joint_->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);
		else if (motion == ConfigurableJointMotion::Limited)
			joint_->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLIMITED);
		else if (motion == ConfigurableJointMotion::Free)
			joint_->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);
		else
			joint_->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);
	}

	void PhysxConfigurableJoint::setAngularXMotion(ConfigurableJointMotion motion)
	{
		if (motion == ConfigurableJointMotion::Locked)
			joint_->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLOCKED);
		else if (motion == ConfigurableJointMotion::Limited)
			joint_->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);
		else if (motion == ConfigurableJointMotion::Free)
			joint_->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
		else
			joint_->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLOCKED);			
	}

	void PhysxConfigurableJoint::setAngularYMotion(ConfigurableJointMotion motion)
	{
		if (motion == ConfigurableJointMotion::Locked)
			joint_->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLOCKED);
		else if (motion == ConfigurableJointMotion::Limited)
			joint_->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
		else if (motion == ConfigurableJointMotion::Free)
			joint_->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
		else
			joint_->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLOCKED);
	}

	void PhysxConfigurableJoint::setAngularZMotion(ConfigurableJointMotion motion)
	{
		if (motion == ConfigurableJointMotion::Locked)
			joint_->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED);
		else if (motion == ConfigurableJointMotion::Limited)
			joint_->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED);
		else if (motion == ConfigurableJointMotion::Free)
			joint_->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eFREE);
		else
			joint_->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED);
	}

	ConfigurableJointMotion PhysxConfigurableJoint::getXMotion()
	{
		auto motion = joint_->getMotion(physx::PxD6Axis::eX);
		if (motion == physx::PxD6Motion::eLOCKED)
			return ConfigurableJointMotion::Locked;
		else if (motion == physx::PxD6Motion::eLIMITED)
			return ConfigurableJointMotion::Limited;
		else if (motion == physx::PxD6Motion::eFREE)
			return ConfigurableJointMotion::Free;
		else
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion PhysxConfigurableJoint::getYMotion()
	{
		auto motion = joint_->getMotion(physx::PxD6Axis::eY);
		if (motion == physx::PxD6Motion::eLOCKED)
			return ConfigurableJointMotion::Locked;
		else if (motion == physx::PxD6Motion::eLIMITED)
			return ConfigurableJointMotion::Limited;
		else if (motion == physx::PxD6Motion::eFREE)
			return ConfigurableJointMotion::Free;
		else
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion PhysxConfigurableJoint::getZMotion()
	{
		auto motion = joint_->getMotion(physx::PxD6Axis::eZ);
		if (motion == physx::PxD6Motion::eLOCKED)
			return ConfigurableJointMotion::Locked;
		else if (motion == physx::PxD6Motion::eLIMITED)
			return ConfigurableJointMotion::Limited;
		else if (motion == physx::PxD6Motion::eFREE)
			return ConfigurableJointMotion::Free;
		else
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion PhysxConfigurableJoint::getAngularXMotion()
	{
		auto motion = joint_->getMotion(physx::PxD6Axis::eTWIST);
		if (motion == physx::PxD6Motion::eLOCKED)
			return ConfigurableJointMotion::Locked;
		else if (motion == physx::PxD6Motion::eLIMITED)
			return ConfigurableJointMotion::Limited;
		else if (motion == physx::PxD6Motion::eFREE)
			return ConfigurableJointMotion::Free;
		else
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion PhysxConfigurableJoint::getAngularYMotion()
	{
		auto motion = joint_->getMotion(physx::PxD6Axis::eSWING1);
		if (motion == physx::PxD6Motion::eLOCKED)
			return ConfigurableJointMotion::Locked;
		else if (motion == physx::PxD6Motion::eLIMITED)
			return ConfigurableJointMotion::Limited;
		else if (motion == physx::PxD6Motion::eFREE)
			return ConfigurableJointMotion::Free;
		else
			return ConfigurableJointMotion::Locked;
	}

	ConfigurableJointMotion PhysxConfigurableJoint::getAngularZMotion()
	{
		auto motion = joint_->getMotion(physx::PxD6Axis::eSWING2);
		if (motion == physx::PxD6Motion::eLOCKED)
			return ConfigurableJointMotion::Locked;
		else if (motion == physx::PxD6Motion::eLIMITED)
			return ConfigurableJointMotion::Limited;
		else if (motion == physx::PxD6Motion::eFREE)
			return ConfigurableJointMotion::Free;
		else
			return ConfigurableJointMotion::Locked;
	}

	void
	PhysxConfigurableJoint::setLowXLimit(float value)
	{
		auto scale = this->context_->getPxPhysics()->getTolerancesScale();
		physx::PxJointLinearLimitPair limit(scale, value, joint_->getLinearLimit(physx::PxD6Axis::eX).upper);
		joint_->setLinearLimit(physx::PxD6Axis::eX, limit);
	}

	void
	PhysxConfigurableJoint::setLowYLimit(float value)
	{
		auto scale = this->context_->getPxPhysics()->getTolerancesScale();
		physx::PxJointLinearLimitPair limit(scale, value, joint_->getLinearLimit(physx::PxD6Axis::eY).upper);
		joint_->setLinearLimit(physx::PxD6Axis::eY, limit);
	}

	void
	PhysxConfigurableJoint::setLowZLimit(float value)
	{
		auto scale = this->context_->getPxPhysics()->getTolerancesScale();
		physx::PxJointLinearLimitPair limit(scale, value, joint_->getLinearLimit(physx::PxD6Axis::eZ).upper);
		joint_->setLinearLimit(physx::PxD6Axis::eZ, limit);
	}

	void
	PhysxConfigurableJoint::setHighXLimit(float value)
	{
		auto scale = this->context_->getPxPhysics()->getTolerancesScale();
		physx::PxJointLinearLimitPair limit(scale, joint_->getLinearLimit(physx::PxD6Axis::eX).lower, value);
		joint_->setLinearLimit(physx::PxD6Axis::eX, limit);
	}

	void
	PhysxConfigurableJoint::setHighYLimit(float value)
	{
		auto scale = this->context_->getPxPhysics()->getTolerancesScale();
		physx::PxJointLinearLimitPair limit(scale, joint_->getLinearLimit(physx::PxD6Axis::eY).lower, value);
		joint_->setLinearLimit(physx::PxD6Axis::eY, limit);
	}

	void
	PhysxConfigurableJoint::setHighZLimit(float value)
	{
		auto scale = this->context_->getPxPhysics()->getTolerancesScale();
		physx::PxJointLinearLimitPair limit(scale, joint_->getLinearLimit(physx::PxD6Axis::eZ).lower, value);
		joint_->setLinearLimit(physx::PxD6Axis::eZ, limit);
	}

	void
	PhysxConfigurableJoint::setDistanceLimit(float distance)
	{
		auto scale = this->context_->getPxPhysics()->getTolerancesScale();
		physx::PxJointLinearLimit limit(scale, distance);
		joint_->setDistanceLimit(limit);
	}

	void
	PhysxConfigurableJoint::setTwistLimit(float min, float max)
	{
		physx::PxJointAngularLimitPair twist(min, max);
		joint_->setTwistLimit(twist);
	}

	void
	PhysxConfigurableJoint::setSwingLimit(float limit_y, float limit_z)
	{
		physx::PxJointLimitCone limit(limit_y, limit_z);
		joint_->setSwingLimit(limit);
	}

	void
	PhysxConfigurableJoint::setPyramidSwingLimit(float min_y, float max_y, float min_z, float max_z)
	{
		physx::PxJointLimitPyramid limit(min_y, max_y, min_z, max_z);
		joint_->setPyramidSwingLimit(limit);
	}

	void
	PhysxConfigurableJoint::setAngularLimit(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
	{
		assert(false);
	}

	void
	PhysxConfigurableJoint::setDriveMotionX(float motion)
	{
		if (motion != 0.0f)
		{
			physx::PxD6JointDrive drive;
			drive.stiffness = motion;
			joint_->setDrive(physx::PxD6Drive::eX, drive);
		}
	}

	void
	PhysxConfigurableJoint::setDriveMotionY(float motion)
	{
		if (motion != 0.0f)
		{
			physx::PxD6JointDrive drive;
			drive.stiffness = motion;
			joint_->setDrive(physx::PxD6Drive::eY, drive);
		}
	}

	void
	PhysxConfigurableJoint::setDriveMotionZ(float motion)
	{
		if (motion != 0.0f)
		{
			physx::PxD6JointDrive drive;
			drive.stiffness = motion;
			joint_->setDrive(physx::PxD6Drive::eZ, drive);
		}
	}

	void 
	PhysxConfigurableJoint::setDriveAngularX(float motion)
	{
		if (motion != 0.0f)
		{
			physx::PxD6JointDrive drive;
			drive.stiffness = motion;
			joint_->setDrive(physx::PxD6Drive::eTWIST, drive);
		}
	}

	void 
	PhysxConfigurableJoint::setDriveAngularY(float motion)
	{
		if (motion != 0.0f)
		{
			physx::PxD6JointDrive drive;
			drive.stiffness = motion;
			joint_->setDrive(physx::PxD6Drive::eSWING, drive);
		}
	}

	void
	PhysxConfigurableJoint::setDriveAngularZ(float motion)
	{
		if (motion != 0.0f)
		{
			physx::PxD6JointDrive drive;
			drive.stiffness = motion;
			joint_->setDrive(physx::PxD6Drive::eSLERP, drive);
		}
	}

	void
	PhysxConfigurableJoint::enableProjection(bool enable) noexcept
	{
		joint_->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, enable);
	}

	void
	PhysxConfigurableJoint::enablePreprocessing(bool enable) noexcept
	{
		joint_->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_PREPROCESSING, !enable);
	}
}