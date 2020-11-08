#include "physx_rigidbody.h"

#include "physx_context.h"
#include "physx_shape.h"

#include <octoon/runtime/except.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
	PhysxRigidbody::PhysxRigidbody(PhysxContext * context, PhysicsRigidbodyDesc desc)
		: context(nullptr)
		, rigidbody_(nullptr)
	{
		physx::PxTransform pose;
		pose.p = physx::PxVec3(desc.translate.x, desc.translate.y, desc.translate.z);
		pose.q = physx::PxQuat(desc.rotation.x, desc.rotation.y, desc.rotation.z, desc.rotation.w);

		auto rigidbody = context->getPxPhysics()->createRigidDynamic(pose);
		if (rigidbody)
		{
			physx::PxRigidBodyExt::setMassAndUpdateInertia(*rigidbody, desc.mass);
			rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, desc.type == PhysicsRigidbodyType::Static);
			rigidbody_ = rigidbody;
		}
		else
		{
			throw  runtime::runtime_error::create("create body failed!");
		}
	}

	PhysxRigidbody::~PhysxRigidbody()
	{
		if (rigidbody_)
		{
			rigidbody_->release();
			rigidbody_ = nullptr;
		}
	}

	void
	PhysxRigidbody::attachShape(std::shared_ptr<PhysicsShape> shape)
	{
		auto pxshape = std::dynamic_pointer_cast<PhysxShape>(shape);
		rigidbody_->attachShape(*pxshape->getPxShape());
		physx::PxRigidBodyExt::setMassAndUpdateInertia(*rigidbody_, rigidbody_->getMass());
		shape_ = pxshape;
	}

	void
	PhysxRigidbody::detachShape(std::shared_ptr<PhysicsShape> shape)
	{
		auto pxshape = std::dynamic_pointer_cast<PhysxShape>(shape);
		rigidbody_->detachShape(*pxshape->getPxShape());
		shape.reset();
		shape = nullptr;
	}

	void
	PhysxRigidbody::setPosition(const math::float3& postion)
	{
		auto pxGlobalPose = rigidbody_->getGlobalPose();
		pxGlobalPose.p = physx::PxVec3(postion.x, postion.y, postion.z);
		rigidbody_->setGlobalPose(pxGlobalPose);
	}

	void
	PhysxRigidbody::setRotation(const math::Quaternion& rotation)
	{
		auto pxGlobalPose = rigidbody_->getGlobalPose();
		pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
		rigidbody_->setGlobalPose(pxGlobalPose);
	}

	void
	PhysxRigidbody::setPositionAndRotation(const math::float3& postion, const math::Quaternion& rotation)
	{
		auto pxGlobalPose = rigidbody_->getGlobalPose();
		pxGlobalPose.p = physx::PxVec3(postion.x, postion.y, postion.z);
		pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
		rigidbody_->setGlobalPose(pxGlobalPose);
	}

	math::float3
	PhysxRigidbody::getPosition()
	{
		auto pxGlobalPose = rigidbody_->getGlobalPose();
		return math::float3(pxGlobalPose.p.x, pxGlobalPose.p.y, pxGlobalPose.p.z);
	}

	math::Quaternion
	PhysxRigidbody::getRotation()
	{
		auto pxGlobalPose = rigidbody_->getGlobalPose();
		return math::Quaternion(pxGlobalPose.q.x, pxGlobalPose.q.y, pxGlobalPose.q.z, pxGlobalPose.q.w);
	}

	void
	PhysxRigidbody::wakeUp()
	{
		rigidbody_->wakeUp();
	}

	void
	PhysxRigidbody::clearForce() noexcept
	{
		rigidbody_->clearForce();
	}

	void
	PhysxRigidbody::clearTorque() noexcept
	{
		rigidbody_->clearTorque();
	}

	void
	PhysxRigidbody::setDynamicFriction(float f)
	{
		shape_->getPxMaterial()->setDynamicFriction(f);
	}

	void
	PhysxRigidbody::setStaticFriction(float f)
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
	PhysxRigidbody::setRestitution(float f)
	{
		shape_->getPxMaterial()->setRestitution(f);
	}

	float
	PhysxRigidbody::getMass() const
	{
		return rigidbody_->getMass();
	}

	float
	PhysxRigidbody::getRestitution() const
	{
		return shape_->getPxMaterial()->getRestitution();
	}

	float
	PhysxRigidbody::getStaticFriction() const
	{
		return shape_->getPxMaterial()->getStaticFriction();
	}

	float
	PhysxRigidbody::getDynamicFriction() const
	{
		return shape_->getPxMaterial()->getDynamicFriction();
	}

	void
	PhysxRigidbody::setMass(float mass)
	{
		physx::PxRigidBodyExt::setMassAndUpdateInertia(*rigidbody_, mass);
	}

	void
	PhysxRigidbody::setLinearDamping(float value)
	{
		rigidbody_->setLinearDamping(value);
	}

	void
	PhysxRigidbody::setAngularDamping(float value)
	{
		rigidbody_->setAngularDamping(value);
	}

	void
	PhysxRigidbody::setLinearVelocity(const math::float3& value)
	{
		rigidbody_->setLinearVelocity(physx::PxVec3(value.x, value.y, value.z));
	}

	void
	PhysxRigidbody::setAngularVelocity(const math::float3& value)
	{
		rigidbody_->setAngularVelocity(physx::PxVec3(value.x, value.y, value.z));
	}

	void
	PhysxRigidbody::setEnableCCD(bool enable)
	{
		rigidbody_->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, enable);
	}

	void
	PhysxRigidbody::setOwnerListener(PhysicsListener* listener)
	{
		rigidbody_->userData = listener;
	}

	void 
	PhysxRigidbody::setKinematic(bool kinematic) noexcept
	{
		rigidbody_->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematic);
	}

	void
	PhysxRigidbody::setSleepThreshold(float threshold) noexcept
	{
		rigidbody_->setSleepThreshold(threshold);
	}

	void
	PhysxRigidbody::setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters) noexcept
	{
		rigidbody_->setSolverIterationCounts(minPositionIters, minVelocityIters);
	}

	physx::PxRigidActor*
	PhysxRigidbody::getPxRigidbody()
	{
		return rigidbody_;
	}
}


