#include "bullet_rigidbody.h"
#include "bullet_context.h"
#include "bullet_shape.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	BulletRigidbody::BulletRigidbody(PhysicsRigidbodyDesc desc)
	{
		btVector3 btv3LocalInertia(0.0f, 0.0f, 0.0f);

		auto _motion = new btDefaultMotionState();

		rigidbody_ = std::make_unique<btRigidBody>(1.0f, _motion, nullptr, btv3LocalInertia);
		rigidbody_->setUserPointer(this);

		this->setPosition(desc.translate);
		this->setRotation(desc.rotation);
		this->setMass(desc.mass);
	}

	BulletRigidbody::~BulletRigidbody()
	{
		delete rigidbody_->getMotionState();
	}

	void
	BulletRigidbody::attachShape(std::shared_ptr<PhysicsShape> shape)
	{
		auto pxshape = std::dynamic_pointer_cast<BulletShape>(shape);
		rigidbody_->setCollisionShape(pxshape->getShape());
		this->setMass(this->getMass());
	}

	void
	BulletRigidbody::detachShape(std::shared_ptr<PhysicsShape> shape)
	{
		auto pxshape = std::dynamic_pointer_cast<BulletShape>(shape);
		rigidbody_->setCollisionShape(nullptr);
		shape = nullptr;
	}

	void
	BulletRigidbody::setPosition(const math::float3& position)
	{
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(position.x, position.y, position.z));
		groundTransform.setRotation(btQuaternion(quaternion_.x, quaternion_.y, quaternion_.z, quaternion_.w));

		this->rigidbody_->getMotionState()->setWorldTransform(groundTransform);
		this->position_ = position;
	}

	void
	BulletRigidbody::setRotation(const math::Quaternion& quaternion)
	{
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(position_.x, position_.y, position_.z));
		groundTransform.setRotation(btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

		this->rigidbody_->getMotionState()->setWorldTransform(groundTransform);
		this->quaternion_ = quaternion;
	}

	void
	BulletRigidbody::setPositionAndRotation(const math::float3& position, const math::Quaternion& quaternion)
	{
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(position.x, position.y, position.z));
		groundTransform.setRotation(btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

		this->rigidbody_->getMotionState()->setWorldTransform(groundTransform);
		this->position_ = position;
		this->quaternion_ = quaternion;
	}

	math::float3
	BulletRigidbody::getPosition()
	{
		auto pose = rigidbody_->getWorldTransform();
		auto origin = pose.getOrigin();
		return math::float3(origin.x(), origin.y(), origin.z());
	}

	math::Quaternion
	BulletRigidbody::getRotation()
	{
		auto pose = rigidbody_->getWorldTransform();
		auto rotation = pose.getRotation();
		return math::Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
	}

	void
	BulletRigidbody::wakeUp()
	{
		rigidbody_->setActivationState(DISABLE_DEACTIVATION);
	}

	void
	BulletRigidbody::clearForce() noexcept
	{
		rigidbody_->clearForces();
	}

	void
	BulletRigidbody::clearTorque() noexcept
	{
	}

	void
	BulletRigidbody::setDynamicFriction(float f)
	{
		rigidbody_->setFriction(f);
	}

	void
	BulletRigidbody::setStaticFriction(float f)
	{
		rigidbody_->setHitFraction(f);
	}

	void
	BulletRigidbody::setGroup(std::uint8_t group)
	{
		rigidbody_->setUserIndex(group);
	}

	void
	BulletRigidbody::setGroupMask(std::uint16_t groupMask)
	{
		rigidbody_->setUserIndex2(groupMask);
	}

	void
	BulletRigidbody::setRestitution(float f)
	{
		rigidbody_->setRestitution(f);
	}

	float
	BulletRigidbody::getMass() const
	{
		return rigidbody_->getMass();
	}

	float
	BulletRigidbody::getRestitution() const
	{
		return rigidbody_->getRestitution();
	}

	float
	BulletRigidbody::getStaticFriction() const
	{
		return rigidbody_->getFriction();
	}

	float
	BulletRigidbody::getDynamicFriction() const
	{
		return rigidbody_->getHitFraction();
	}

	void
	BulletRigidbody::setMass(float mass)
	{
		btVector3 inertia(0.0f, 0.0f, 0.0f);

		btCollisionShape* collision = rigidbody_->getCollisionShape();
		if (collision)
			collision->calculateLocalInertia(mass, inertia);

		rigidbody_->setMassProps(mass, inertia);
	}

	void
	BulletRigidbody::setLinearDamping(float value)
	{
		rigidbody_->setDamping(value, rigidbody_->getAngularDamping());
	}

	void
	BulletRigidbody::setAngularDamping(float value)
	{
		rigidbody_->setDamping(rigidbody_->getLinearDamping(), value);
	}

	void
	BulletRigidbody::setLinearVelocity(const math::float3& value)
	{
		rigidbody_->setLinearVelocity(btVector3(value.x, value.y, value.z));
	}

	void
	BulletRigidbody::setAngularVelocity(const math::float3& value)
	{
		rigidbody_->setAngularVelocity(btVector3(value.x, value.y, value.z));
	}

	void
	BulletRigidbody::setEnableCCD(bool enable)
	{
		//rigidbody_->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, enable);
	}

	void
	BulletRigidbody::setOwnerListener(PhysicsListener* listener)
	{
		rigidbody_->setUserPointer(listener);
	}

	void 
	BulletRigidbody::setKinematic(bool kinematic) noexcept
	{
		if (kinematic)
			rigidbody_->setCollisionFlags(rigidbody_->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		else
			rigidbody_->setCollisionFlags(rigidbody_->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	}

	void
	BulletRigidbody::setSleepThreshold(float threshold) noexcept
	{
		rigidbody_->setSleepingThresholds(threshold, threshold);
	}

	void
	BulletRigidbody::setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters) noexcept
	{
		//rigidbody_->setSolverIterationCounts(minPositionIters, minVelocityIters);
	}

	btRigidBody*
	BulletRigidbody::getRigidbody()
	{
		return rigidbody_.get();
	}
}