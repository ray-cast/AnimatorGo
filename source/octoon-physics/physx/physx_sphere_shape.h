#ifndef OCTOON_PHYSX_SPHERE_SHAPE_H_
#define OCTOON_PHYSX_SPHERE_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>


#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_sphere_shape.h>

#include "physx_shape.h"

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxSphereShape: public PhysicsSphereShape, public PhysxShape
		{
		public:
			PhysxSphereShape(PhysxContext* context, float radius);
			virtual ~PhysxSphereShape() noexcept;

			virtual math::float3 getCenter() override;
			virtual void setCenter(const math::float3& center) override;

			virtual math::Quaternion getQuaternion() override;
			virtual void setQuaternion(const math::Quaternion& center) override;

			virtual void setContactOffset(float offset) override;
			virtual float getContactOffset() const override;

			virtual void setRestOffset(float offset) override;
			virtual float getRestOffset() const override;

			virtual float getRadius() override;
			virtual void setRadius(float radius) override;

			virtual physx::PxShape* getPxShape() override;
			virtual physx::PxMaterial* getPxMaterial() override;

		private:
			PhysxSphereShape(const PhysxSphereShape&) noexcept = delete;
			PhysxSphereShape& operator=(const PhysxSphereShape&) noexcept = delete;
		private:
			PhysicsContext* context_;
			physx::PxShape* px_shape;
			physx::PxMaterial* px_material;
		};
	}
}

#endif