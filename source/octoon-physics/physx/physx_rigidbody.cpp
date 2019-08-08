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
			if (rigidbody)
			{
				rigidbody->setMass(desc.mass);
				rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, desc.type == PhysicsRigidbodyType::Static);
				if (desc.type != PhysicsRigidbodyType::Static)
					rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
				px_rigidbody = rigidbody;
			}
			else
			{
				throw  runtime::runtime_error::create("create body failed!");
			}
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

		void
		PhysxRigidbody::clearForce() noexcept
		{
			px_rigidbody->clearForce();
		}

		void
		PhysxRigidbody::clearTorque() noexcept
		{
			px_rigidbody->clearTorque();
		}

		void PhysxRigidbody::setDynamicFriction(float f)
		{
			shape_->getPxMaterial()->setDynamicFriction(f);
		}

		float PhysxRigidbody::getDynamicFriction() const
		{
			return shape_->getPxMaterial()->getDynamicFriction();
		}

		void PhysxRigidbody::setStaticFriction(float f)
		{
			shape_->getPxMaterial()->setStaticFriction(f);
		}

		void
		PhysxRigidbody::setGroup(std::uint8_t group)
		{
			physx::PxFilterData data = shape_->getPxShape()->getSimulationFilterData();
			data.word0 = group;
			shape_->getPxShape()->setSimulationFilterData(data);
		}

		void
		PhysxRigidbody::setGroupMask(std::uint16_t groupMask)
		{
			physx::PxFilterData data = shape_->getPxShape()->getSimulationFilterData();
			data.word2 = groupMask;
			shape_->getPxShape()->setSimulationFilterData(data);
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

		void
		PhysxRigidbody::setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters) noexcept
		{
			px_rigidbody->setSolverIterationCounts(minPositionIters, minVelocityIters);
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
			return shape_->getPxMaterial()->getStaticFriction();
		}

		void PhysxRigidbody::setRestitution(float f)
		{
			shape_->getPxMaterial()->setRestitution(f);
		}

		float PhysxRigidbody::getRestitution() const
		{
			return shape_->getPxMaterial()->getRestitution();
		}

		void PhysxRigidbody::attachShape(std::shared_ptr<PhysicsShape> shape)
		{
			auto pxshape = std::dynamic_pointer_cast<PhysxShape>(shape);
			px_rigidbody->attachShape(*pxshape->getPxShape());
			shape_ = pxshape;
			if (px_rigidbody->getRigidBodyFlags() & physx::PxRigidBodyFlag::eENABLE_CCD)
			{
				physx::PxFilterData data = shape_->getPxShape()->getSimulationFilterData();
				data.word3 = physx::PxRigidBodyFlag::eENABLE_CCD;
				shape_->getPxShape()->setSimulationFilterData(data);
			}
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


