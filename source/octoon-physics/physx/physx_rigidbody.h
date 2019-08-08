#ifndef OCTOON_PHYSX_RIGIDBODY_H_
#define OCTOON_PHYSX_RIGIDBODY_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_rigidbody.h>

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxRigidbody : public PhysicsRigidbody
		{
		public:
			PhysxRigidbody(PhysxContext* context, PhysicsRigidbodyDesc desc);
			virtual ~PhysxRigidbody();

			virtual void setGroup(std::uint8_t group) override;
			virtual void setGroupMask(std::uint16_t groupMask) override;

			virtual void setOwnerListener(PhysicsListener* listener) override;

			virtual void setKinematic(bool kinematic) noexcept override;
			virtual void setSleepThreshold(float threshold) noexcept override;
			virtual void setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters = 1) noexcept override;

			virtual void setPosition(math::float3 postion) override;
			virtual void setRotation(math::Quaternion rotation) override;
			virtual void setPositionAndRotation(math::float3 postion, math::Quaternion rotation) override;

			virtual void setMass(float f) override;
			virtual void setDynamicFriction(float f) override;
			virtual void setStaticFriction(float f) override;
			virtual void setRestitution(float f) override;

			virtual void setLinearDamping(float value) override;
			virtual void setAngularDamping(float value) override;

			virtual float getDynamicFriction() const override;
			virtual float getStaticFriction() const override;
			virtual float getRestitution() const override;

			virtual math::float3 getPosition() override;
			virtual math::Quaternion getRotation() override;

			virtual void wakeUp() override;
			virtual void clearForce() noexcept override;
			virtual void clearTorque() noexcept override;			

			virtual void attachShape(std::shared_ptr<PhysicsShape> shape) override;
			virtual void detachShape(std::shared_ptr<PhysicsShape> shape) override;

			physx::PxRigidActor* getPxRigidbody();

		private:
			PhysxRigidbody(const PhysxRigidbody&) noexcept = delete;
			PhysxRigidbody& operator=(const PhysxRigidbody&) noexcept = delete;
		private:
			PhysxContext* context;
			physx::PxRigidDynamic* px_rigidbody;

			std::shared_ptr<PhysxShape> shape_;
		};
	}
}

#endif