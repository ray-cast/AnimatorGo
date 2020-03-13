#ifndef OCTOON_PHYSX_CAPSULE_SHAPE_H_
#define OCTOON_PHYSX_CAPSULE_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>
#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_capsule_shape.h>

#include "physx_shape.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxCapsuleShape : public PhysicsCapsuleShape, public PhysxShape
		{
		public:
			PhysxCapsuleShape(PhysxContext* context, float radius, float height);
			virtual ~PhysxCapsuleShape() noexcept;

			virtual math::float3 getCenter() override;
			virtual void setCenter(const math::float3& center) override;

			virtual math::Quaternion getQuaternion() override;
			virtual void setQuaternion(const math::Quaternion& center) override;

			virtual void setContactOffset(float offset) override;
			virtual float getContactOffset() const override;

			virtual void setRestOffset(float offset) override;
			virtual float getRestOffset() const override;

			virtual void setRadius(float width) override;
			virtual void setHeight(float height) override;

			virtual float getRadius() const override;
			virtual float getHeight() const override;

			virtual physx::PxShape* getPxShape() override;
			virtual physx::PxMaterial* getPxMaterial() override;

		private:
			PhysxCapsuleShape(const PhysxCapsuleShape&) noexcept = delete;
			PhysxCapsuleShape& operator=(const PhysxCapsuleShape&) noexcept = delete;

		private:
			float radius_;
			float height_;

			PhysicsContext* context_;
			physx::PxShape* px_shape;
			physx::PxMaterial* px_material;
		};
	}
}

#endif