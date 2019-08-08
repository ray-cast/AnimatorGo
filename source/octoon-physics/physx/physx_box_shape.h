#ifndef OCTOON_PHYSX_BOX_SHAPE_H_
#define OCTOON_PHYSX_BOX_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>
#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_box_shape.h>

#include "physx_shape.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxBoxShape : public PhysicsBoxShape, public PhysxShape
		{
		public:
			PhysxBoxShape(PhysxContext* context, float hx, float hy, float hz);
			virtual ~PhysxBoxShape() noexcept;

			virtual math::float3 getCenter() override;
			virtual void setCenter(math::float3 center) override;

			virtual void setContactOffset(float offset) override;
			virtual float getContactOffset() const override;

			virtual void setRestOffset(float offset) override;
			virtual float getRestOffset() const override;

			virtual void setWidth(float width) override;
			virtual void setHeight(float height) override;
			virtual void setDepth(float depth) override;

			virtual float getWidth() const override;
			virtual float getHeight() const override;
			virtual float getDepth() const override;

			virtual physx::PxShape* getPxShape() override;
			virtual physx::PxMaterial* getPxMaterial() override;

		private:
			PhysxBoxShape(const PhysxBoxShape&) noexcept = delete;
			PhysxBoxShape& operator=(const PhysxBoxShape&) noexcept = delete;

		private:
			float width_;
			float height_;
			float depth_;

			PhysicsContext* context_;
			physx::PxShape* px_shape;
			physx::PxMaterial* px_material;
		};
	}
}

#endif