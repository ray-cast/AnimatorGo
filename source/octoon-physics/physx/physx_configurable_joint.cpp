#include "physx_configurable_joint.h"

#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxConfigurableJoint::PhysxConfigurableJoint(PhysxContext * context) noexcept
			:context(nullptr), joint(nullptr)
		{
		}

		PhysxConfigurableJoint::~PhysxConfigurableJoint() noexcept
		{
			if (joint)
			{
				joint->release();
				joint = nullptr;
			}
		}

		void PhysxConfigurableJoint::connect(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
		{
			std::shared_ptr<PhysxRigidbody> px_lhs = std::dynamic_pointer_cast<PhysxRigidbody>(lhs);
			std::shared_ptr<PhysxRigidbody> px_rhs = std::dynamic_pointer_cast<PhysxRigidbody>(rhs);
			joint = physx::PxD6JointCreate(*context->getPxPhysics(),
				px_lhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f),
				px_rhs->getPxRigidbody(), physx::PxTransform(0.f, 0.f, 0.f));
		}

		void PhysxConfigurableJoint::setAngularXMotion(ConfigurableJointMotion motion)
		{
			if (motion == ConfigurableJointMotion::Locked)
			{
				joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLOCKED);
			}
			else if (motion == ConfigurableJointMotion::Limited)
			{
				joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);
			}
			else if (motion == ConfigurableJointMotion::Free)
			{
				joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
			}
			else
			{
				joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLOCKED);
			}
			
		}

		ConfigurableJointMotion PhysxConfigurableJoint::getAngularXMotion()
		{
			auto motion = joint->getMotion(physx::PxD6Axis::eTWIST);
			if (motion == physx::PxD6Motion::eLOCKED)
			{
				return ConfigurableJointMotion::Locked;
			}
			else if (motion == physx::PxD6Motion::eLIMITED)
			{
				return ConfigurableJointMotion::Limited;
			}
			else if (motion == physx::PxD6Motion::eFREE)
			{
				return ConfigurableJointMotion::Free;
			}
			else
			{
				return ConfigurableJointMotion::Locked;
			}
		}

		void PhysxConfigurableJoint::setAngularYMotion(ConfigurableJointMotion motion)
		{
			if (motion == ConfigurableJointMotion::Locked)
			{
				joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLOCKED);
			}
			else if (motion == ConfigurableJointMotion::Limited)
			{
				joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
			}
			else if (motion == ConfigurableJointMotion::Free)
			{
				joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
			}
			else
			{
				joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLOCKED);
			}
		}

		ConfigurableJointMotion PhysxConfigurableJoint::getAngularYMotion()
		{
			auto motion = joint->getMotion(physx::PxD6Axis::eSWING1);
			if (motion == physx::PxD6Motion::eLOCKED)
			{
				return ConfigurableJointMotion::Locked;
			}
			else if (motion == physx::PxD6Motion::eLIMITED)
			{
				return ConfigurableJointMotion::Limited;
			}
			else if (motion == physx::PxD6Motion::eFREE)
			{
				return ConfigurableJointMotion::Free;
			}
			else
			{
				return ConfigurableJointMotion::Locked;
			}
		}

		void PhysxConfigurableJoint::setAngularZMotion(ConfigurableJointMotion motion)
		{
			if (motion == ConfigurableJointMotion::Locked)
			{
				joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED);
			}
			else if (motion == ConfigurableJointMotion::Limited)
			{
				joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED);
			}
			else if (motion == ConfigurableJointMotion::Free)
			{
				joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eFREE);
			}
			else
			{
				joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED);
			}
		}

		ConfigurableJointMotion PhysxConfigurableJoint::getAngularZMotion()
		{
			auto motion = joint->getMotion(physx::PxD6Axis::eSWING2);
			if (motion == physx::PxD6Motion::eLOCKED)
			{
				return ConfigurableJointMotion::Locked;
			}
			else if (motion == physx::PxD6Motion::eLIMITED)
			{
				return ConfigurableJointMotion::Limited;
			}
			else if (motion == physx::PxD6Motion::eFREE)
			{
				return ConfigurableJointMotion::Free;
			}
			else
			{
				return ConfigurableJointMotion::Locked;
			}
		}

		void PhysxConfigurableJoint::setXMotion(ConfigurableJointMotion motion)
		{
			if (motion == ConfigurableJointMotion::Locked)
			{
				joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
			}
			else if (motion == ConfigurableJointMotion::Limited)
			{
				joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLIMITED);
			}
			else if (motion == ConfigurableJointMotion::Free)
			{
				joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
			}
			else
			{
				joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
			}
		}

		ConfigurableJointMotion PhysxConfigurableJoint::getXMotion()
		{
			auto motion = joint->getMotion(physx::PxD6Axis::eX);
			if (motion == physx::PxD6Motion::eLOCKED)
			{
				return ConfigurableJointMotion::Locked;
			}
			else if (motion == physx::PxD6Motion::eLIMITED)
			{
				return ConfigurableJointMotion::Limited;
			}
			else if (motion == physx::PxD6Motion::eFREE)
			{
				return ConfigurableJointMotion::Free;
			}
			else
			{
				return ConfigurableJointMotion::Locked;
			}
		}

		void PhysxConfigurableJoint::setYMotion(ConfigurableJointMotion motion)
		{
			if (motion == ConfigurableJointMotion::Locked)
			{
				joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
			}
			else if (motion == ConfigurableJointMotion::Limited)
			{
				joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLIMITED);
			}
			else if (motion == ConfigurableJointMotion::Free)
			{
				joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
			}
			else
			{
				joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
			}
		}

		ConfigurableJointMotion PhysxConfigurableJoint::getYMotion()
		{
			auto motion = joint->getMotion(physx::PxD6Axis::eY);
			if (motion == physx::PxD6Motion::eLOCKED)
			{
				return ConfigurableJointMotion::Locked;
			}
			else if (motion == physx::PxD6Motion::eLIMITED)
			{
				return ConfigurableJointMotion::Limited;
			}
			else if (motion == physx::PxD6Motion::eFREE)
			{
				return ConfigurableJointMotion::Free;
			}
			else
			{
				return ConfigurableJointMotion::Locked;
			}
		}

		void PhysxConfigurableJoint::setZMotion(ConfigurableJointMotion motion)
		{
			if (motion == ConfigurableJointMotion::Locked)
			{
				joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);
			}
			else if (motion == ConfigurableJointMotion::Limited)
			{
				joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLIMITED);
			}
			else if (motion == ConfigurableJointMotion::Free)
			{
				joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);
			}
			else
			{
				joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);
			}
		}

		ConfigurableJointMotion PhysxConfigurableJoint::getZMotion()
		{
			auto motion = joint->getMotion(physx::PxD6Axis::eZ);
			if (motion == physx::PxD6Motion::eLOCKED)
			{
				return ConfigurableJointMotion::Locked;
			}
			else if (motion == physx::PxD6Motion::eLIMITED)
			{
				return ConfigurableJointMotion::Limited;
			}
			else if (motion == physx::PxD6Motion::eFREE)
			{
				return ConfigurableJointMotion::Free;
			}
			else
			{
				return ConfigurableJointMotion::Locked;
			}
		}

	}
}
