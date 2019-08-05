#include "physx_rigidbody.h"

#include "physx_context.h"
#include "physx_shape.h"

#include <octoon/runtime/except.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxRigidbody::PhysxRigidbody(PhysxContext * context, PhysicsRigidbodyDesc desc)
			: context(nullptr)
			, px_rigidbody(nullptr)
		{
			physx::PxTransform pose;
			pose.p = physx::PxVec3(desc.translate.x, desc.translate.y, desc.translate.z);
			pose.q = physx::PxQuat(desc.rotation.x, desc.rotation.y, desc.rotation.z, desc.rotation.w);

			auto rigidbody = context->getPxPhysics()->createRigidDynamic(pose);
			if (!rigidbody)
				throw  runtime::runtime_error::create("create body failed!");
			
			rigidbody->setMass(desc.mass);
			rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, desc.type == PhysicsRigidbodyType::Static);

			px_rigidbody = rigidbody;
		}

		PhysxRigidbody::~PhysxRigidbody()
		{
			if (px_rigidbody)
			{
				px_rigidbody->release();
				px_rigidbody = nullptr;
			}
		}

		math::float3 PhysxRigidbody::getPosition()
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			return math::float3(pxGlobalPose.p.x, pxGlobalPose.p.y, pxGlobalPose.p.z);
		}
		void PhysxRigidbody::setPosition(math::float3 postion)
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			pxGlobalPose.p = physx::PxVec3(postion.x, postion.y, postion.z);
			px_rigidbody->setGlobalPose(pxGlobalPose);
		}
		math::Quaternion PhysxRigidbody::getRotation()
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			return math::Quaternion(pxGlobalPose.q.x, pxGlobalPose.q.y, pxGlobalPose.q.z, pxGlobalPose.q.w);
		}

		void PhysxRigidbody::setRotation(math::Quaternion rotation)
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
			px_rigidbody->setGlobalPose(pxGlobalPose);
		}

		void PhysxRigidbody::setPositionAndRotation(math::float3 postion, math::Quaternion rotation)
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			pxGlobalPose.p = physx::PxVec3(postion.x, postion.y, postion.z);
			pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
			px_rigidbody->setGlobalPose(pxGlobalPose);
		}

		void
		PhysxRigidbody::wakeUp()
		{
			px_rigidbody->wakeUp();
		}

		void PhysxRigidbody::setDynamicFriction(float f)
		{
			if (shape_)
				shape_->getPxMaterial()->setDynamicFriction(f);
		}

		float PhysxRigidbody::getDynamicFriction() const
		{
			if (shape_)
				return shape_->getPxMaterial()->getDynamicFriction();
			else
				return 0.f;
		}

		void PhysxRigidbody::setStaticFriction(float f)
		{
			if (shape_)
				shape_->getPxMaterial()->setStaticFriction(f);
		}

		void
		PhysxRigidbody::setGroup(std::uint8_t group)
		{
			physx::PxSetGroup(*px_rigidbody, group);
		}

		void
		PhysxRigidbody::setGroupMask(std::uint16_t groupMask)
		{
			physx::PxGroupsMask mask;
			mask.bits0 = groupMask;
			mask.bits1 = 0;
			mask.bits2 = 0;
			mask.bits3 = 0;
			physx::PxSetGroupsMask(*px_rigidbody, mask);
		}

		void
		PhysxRigidbody::setOwnerListener(PhysicsListener* listener)
		{
			px_rigidbody->userData = listener;
		}

		void 
		PhysxRigidbody::setKinematic(bool kinematic) noexcept
		{
			px_rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematic);
		}

		void
		PhysxRigidbody::setSleepThreshold(float threshold) noexcept
		{
			px_rigidbody->setSleepThreshold(threshold);
		}

		void PhysxRigidbody::setMass(float f)
		{
			px_rigidbody->setMass(f);
		}

		void PhysxRigidbody::setLinearDamping(float value)
		{
			px_rigidbody->setLinearDamping(value);
		}

		void PhysxRigidbody::setAngularDamping(float value)
		{
			px_rigidbody->setAngularDamping(value);
		}

		float PhysxRigidbody::getStaticFriction() const
		{
			if (shape_)
				return shape_->getPxMaterial()->getStaticFriction();
			else
				return 0.f;
		}

		void PhysxRigidbody::setRestitution(float f)
		{
			if (shape_)
			{
				shape_->getPxMaterial()->setRestitution(f);
			}
		}

		float PhysxRigidbody::getRestitution() const
		{
			if (shape_)
				return shape_->getPxMaterial()->getRestitution();
			else
				return 0.f;
		}

		void PhysxRigidbody::attachShape(std::shared_ptr<PhysicsShape> shape)
		{
			auto pxshape = std::dynamic_pointer_cast<PhysxShape>(shape);
			px_rigidbody->attachShape(*pxshape->getPxShape());
		}

		void PhysxRigidbody::detachShape(std::shared_ptr<PhysicsShape> shape)
		{
			auto pxshape = std::dynamic_pointer_cast<PhysxShape>(shape);
			px_rigidbody->detachShape(*pxshape->getPxShape());
			shape.reset();
			shape = nullptr;
		}

		physx::PxRigidActor * PhysxRigidbody::getPxRigidbody()
		{
			return px_rigidbody;
		}
	}
}


