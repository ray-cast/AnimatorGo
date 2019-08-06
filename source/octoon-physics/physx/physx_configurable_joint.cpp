#include "physx_configurable_joint.h"
#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxConfigurableJoint::PhysxConfigurableJoint(PhysxContext* context) noexcept
			: context_(context)
			, joint_(nullptr)
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

		void PhysxConfigurableJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
		{
			std::shared_ptr<PhysxRigidbody> px_lhs = std::dynamic_pointer_cast<PhysxRigidbody>(lhs);
			std::shared_ptr<PhysxRigidbody> px_rhs = std::dynamic_pointer_cast<PhysxRigidbody>(rhs);
			if (joint_)
			{
				joint_->setActors(px_lhs->getPxRigidbody(), px_rhs->getPxRigidbody());
			}
			else
			{
				joint_ = physx::PxD6JointCreate(*context_->getPxPhysics(), 
					px_lhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f), 
					px_rhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f));
			}
		}

		void
		PhysxConfigurableJoint::setRigidATransform(const math::float3& position, const math::Quaternion& rotation) noexcept
		{
			physx::PxTransform pxGlobalPose;
			pxGlobalPose.p = physx::PxVec3(position.x, position.y, position.z);
			pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

			joint_->setLocalPose(physx::PxJointActorIndex::eACTOR0, pxGlobalPose);
		}

		void
		PhysxConfigurableJoint::setRigidBTransform(const math::float3& position, const math::Quaternion& rotation) noexcept
		{
			physx::PxTransform pxGlobalPose;
			pxGlobalPose.p = physx::PxVec3(position.x, position.y, position.z);
			pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

			joint_->setLocalPose(physx::PxJointActorIndex::eACTOR1, pxGlobalPose);
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
			auto limit = physx::PxJointLinearLimitPair(scale, value, joint_->getLinearLimit(physx::PxD6Axis::eX).upper);
			joint_->setLinearLimit(physx::PxD6Axis::eX, limit);
		}

		void
		PhysxConfigurableJoint::setLowYLimit(float value)
		{
			auto scale = this->context_->getPxPhysics()->getTolerancesScale();
			auto limit = physx::PxJointLinearLimitPair(scale, value, joint_->getLinearLimit(physx::PxD6Axis::eY).upper);
			joint_->setLinearLimit(physx::PxD6Axis::eY, limit);
		}

		void
		PhysxConfigurableJoint::setLowZLimit(float value)
		{
			auto scale = this->context_->getPxPhysics()->getTolerancesScale();
			auto limit = physx::PxJointLinearLimitPair(scale, value, joint_->getLinearLimit(physx::PxD6Axis::eZ).upper);
			joint_->setLinearLimit(physx::PxD6Axis::eZ, limit);
		}

		void
		PhysxConfigurableJoint::setHighXLimit(float value)
		{
			auto scale = this->context_->getPxPhysics()->getTolerancesScale();
			auto limit = physx::PxJointLinearLimitPair(scale, joint_->getLinearLimit(physx::PxD6Axis::eX).lower, value);
			joint_->setLinearLimit(physx::PxD6Axis::eX, limit);
		}

		void
		PhysxConfigurableJoint::setHighYLimit(float value)
		{
			auto scale = this->context_->getPxPhysics()->getTolerancesScale();
			auto limit = physx::PxJointLinearLimitPair(scale, joint_->getLinearLimit(physx::PxD6Axis::eY).lower, value);
			joint_->setLinearLimit(physx::PxD6Axis::eY, limit);
		}

		void
		PhysxConfigurableJoint::setHighZLimit(float value)
		{
			auto scale = this->context_->getPxPhysics()->getTolerancesScale();
			auto limit = physx::PxJointLinearLimitPair(scale, joint_->getLinearLimit(physx::PxD6Axis::eZ).lower, value);
			joint_->setLinearLimit(physx::PxD6Axis::eZ, limit);
		}

		void
		PhysxConfigurableJoint::setLowAngularXLimit(float value)
		{
			auto twist = physx::PxJointAngularLimitPair(value, joint_->getTwistLimit().upper);
			joint_->setTwistLimit(twist);
		}

		void
		PhysxConfigurableJoint::setHighAngularXLimit(float value)
		{
			auto twist = physx::PxJointAngularLimitPair(joint_->getTwistLimit().lower, value);
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
	}
}