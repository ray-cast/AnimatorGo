#ifndef OCTOON_BULLET_SPHERE_SHAPE_H_
#define OCTOON_BULLET_SPHERE_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>
#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_sphere_shape.h>

#include "bullet_shape.h"
#include "bullet_type.h"

namespace octoon
{
	class OCTOON_EXPORT BulletSphereShape final : public PhysicsSphereShape, public BulletShape
	{
	public:
		BulletSphereShape(float radius);
		virtual ~BulletSphereShape() noexcept;

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

		virtual btCollisionShape* getShape() const override;

	private:
		BulletSphereShape(const BulletSphereShape&) noexcept = delete;
		BulletSphereShape& operator=(const BulletSphereShape&) noexcept = delete;

	private:
		std::unique_ptr<btSphereShape> shape_;
	};
}

#endif