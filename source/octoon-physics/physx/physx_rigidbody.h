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

			virtual math::float3 getPosition() override;
			virtual void setPosition(math::float3 postion) override;
			virtual math::Quaternion getRotation() override;
			virtual void setRotation(math::Quaternion rotation) override;
			virtual void setPositionAndRotation(math::float3 postion, math::Quaternion rotation) override;

			virtual void setDynamicFriction(float f) override;
			virtual float getDynamicFriction() const override;
			virtual void setStaticFriction(float f) override;
			virtual float getStaticFriction() const override;
			virtual void setRestitution(float f) override;
			virtual float getRestitution() const override;

			virtual void attachShape(std::shared_ptr<PhysicsShape> shape) override;
			virtual void detachShape(std::shared_ptr<PhysicsShape> shape) override;

			physx::PxRigidActor* getPxRigidbody();

		private:
			PhysxRigidbody(const PhysxRigidbody&) noexcept = delete;
			PhysxRigidbody& operator=(const PhysxRigidbody&) noexcept = delete;
		private:
			PhysxContext* context;
			physx::PxRigidActor* px_rigidbody;

			std::shared_ptr<PhysxShape> shape_;
		};
	}
}

#endif