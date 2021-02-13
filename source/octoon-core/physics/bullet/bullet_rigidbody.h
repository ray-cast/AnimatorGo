#ifndef OCTOON_BULLET_RIGIDBODY_H_
#define OCTOON_BULLET_RIGIDBODY_H_

#include <memory>
#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_rigidbody.h>

#include "bullet_type.h"

namespace octoon
{
	class OCTOON_EXPORT BulletRigidbody final : public PhysicsRigidbody
	{
	public:
		BulletRigidbody(PhysicsRigidbodyDesc desc);
		~BulletRigidbody() override;

		void attachShape(std::shared_ptr<PhysicsShape> shape) override;
		void detachShape(std::shared_ptr<PhysicsShape> shape) override;

		void setGroup(std::uint8_t group) override;
		void setGroupMask(std::uint16_t groupMask) override;

		void setEnableCCD(bool enable) override;

		void setOwnerListener(PhysicsListener* listener) override;

		void setKinematic(bool kinematic) noexcept override;
		void setSleepThreshold(float threshold) noexcept override;
		void setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters = 1) noexcept override;

		void setPosition(const math::float3& postion) override;
		void setRotation(const math::Quaternion& rotation) override;
		void setPositionAndRotation(const math::float3& postion, const math::Quaternion& rotation) override;

		void setMass(float f) override;
		void setDynamicFriction(float f) override;
		void setStaticFriction(float f) override;
		void setRestitution(float f) override;

		void setLinearDamping(float value) override;
		void setAngularDamping(float value) override;

		void setLinearVelocity(const math::float3& value) override;
		void setAngularVelocity(const math::float3& value) override;

		float getMass() const override;
		float getDynamicFriction() const override;
		float getStaticFriction() const override;
		float getRestitution() const override;

		math::float3 getPosition() override;
		math::Quaternion getRotation() override;

		void wakeUp() override;
		void clearForce() noexcept override;
		void clearTorque() noexcept override;			

		btRigidBody* getRigidbody();

	private:
		BulletRigidbody(const BulletRigidbody&) noexcept = delete;
		BulletRigidbody& operator=(const BulletRigidbody&) noexcept = delete;

	private:
		math::float3 position_;
		math::Quaternion quaternion_;

		std::unique_ptr<btRigidBody> rigidbody_;
	};
}

#endif