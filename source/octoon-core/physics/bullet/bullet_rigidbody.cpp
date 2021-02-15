#include "bullet_rigidbody.h"
#include "bullet_context.h"
#include "bullet_shape.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	BulletRigidbody::BulletRigidbody(PhysicsRigidbodyDesc desc)
		: position_(desc.position.x, desc.position.y, desc.position.z)
		, quaternion_(desc.rotation.x, desc.rotation.y, desc.rotation.z, desc.rotation.w)
		, mass_(desc.mass)
	{
		btVector3 localInertia(0.0f, 0.0f, 0.0f);
		btVector3 position(position_.x, position_.y, position_.z);
		btQuaternion quaternion(quaternion_.x, quaternion_.y, quaternion_.z, quaternion_.w);

		rigidbody_ = std::make_unique<btRigidBody>(1.0f, new btDefaultMotionState(btTransform(quaternion, position)), nullptr, localInertia);
		rigidbody_->setUserPointer(this);
		rigidbody_->setMassProps(desc.mass, localInertia);
		rigidbody_->updateInertiaTensor();

		if (desc.type == PhysicsRigidbodyType::Static)
			this->setKinematic(true);
	}

	BulletRigidbody::~BulletRigidbody()
	{
		delete rigidbody_->getMotionState();
	}

	void
	BulletRigidbody::attachShape(std::shared_ptr<PhysicsShape> shape)
	{
		rigidbody_->setCollisionShape(std::dynamic_pointer_cast<BulletShape>(shape)->getShape());
		this->setMass(this->getMass());
	}

	void
	BulletRigidbody::detachShape(std::shared_ptr<PhysicsShape> shape)
	{
		rigidbody_->setCollisionShape(nullptr);
		this->setMass(this->getMass());
	}

	void
	BulletRigidbody::setPosition(const math::float3& position)
	{
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(position.x, position.y, position.z));
		groundTransform.setRotation(btQuaternion(quaternion_.x, quaternion_.y, quaternion_.z, quaternion_.w));

		this->position_ = position;
		this->rigidbody_->getMotionState()->setWorldTransform(groundTransform);
	}

	void
	BulletRigidbody::setRotation(const math::Quaternion& quaternion)
	{
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(position_.x, position_.y, position_.z));
		groundTransform.setRotation(btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

		this->quaternion_ = quaternion;
		this->rigidbody_->getMotionState()->setWorldTransform(groundTransform);
	}

	void
	BulletRigidbody::setPositionAndRotation(const math::float3& position, const math::Quaternion& quaternion)
	{
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(position.x, position.y, position.z));
		groundTransform.setRotation(btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

		this->position_ = position;
		this->quaternion_ = quaternion;
		this->rigidbody_->getMotionState()->setWorldTransform(groundTransform);
	}

	math::float3
	BulletRigidbody::getPosition()
	{
		btTransform transform;
		rigidbody_->getMotionState()->getWorldTransform(transform);
		auto origin = transform.getOrigin();
		this->position_.set(origin.x(), origin.y(), origin.z());
		return this->position_;
	}

	math::Quaternion
	BulletRigidbody::getRotation()
	{
		btTransform transform;
		rigidbody_->getMotionState()->getWorldTransform(transform);
		auto rotation = transform.getRotation();
		this->quaternion_.set(rotation.x(), rotation.y(), rotation.z(), rotation.w());
		return this->quaternion_;
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
		rigidbody_->setHitFraction(f);
	}

	void
	BulletRigidbody::setStaticFriction(float f)
	{
		rigidbody_->setFriction(f);
	}

	void
	BulletRigidbody::setGroup(std::uint8_t group)
	{
		rigidbody_->setUserIndex(group);
		rigidbody_->setUserIndex3(true);
	}

	void
	BulletRigidbody::setGroupMask(std::uint16_t groupMask)
	{
		rigidbody_->setUserIndex2(groupMask);
		rigidbody_->setUserIndex3(true);
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

		if (mass != 0.0f)
		{
			btCollisionShape* collision = rigidbody_->getCollisionShape();
			if (collision)
				collision->calculateLocalInertia(mass, inertia);
		}

		this->mass_ = mass;
		this->rigidbody_->setMassProps(mass, inertia);
		this->rigidbody_->updateInertiaTensor();
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
		{
			this->setMass(0.0f);
			this->rigidbody_->setCollisionFlags(rigidbody_->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			this->rigidbody_->setActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			this->setMass(mass_);
			this->rigidbody_->setCollisionFlags(rigidbody_->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		}
	}

	void
	BulletRigidbody::setSleepThreshold(float threshold) noexcept
	{
		rigidbody_->setSleepingThresholds(threshold, threshold);
	}

	void
	BulletRigidbody::setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters) noexcept
	{
	}

	btRigidBody*
	BulletRigidbody::getRigidbody()
	{
		return rigidbody_.get();
	}
}