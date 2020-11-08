#ifndef OCTOON_BULLET_CAPSULE_SHAPE_H_
#define OCTOON_BULLET_CAPSULE_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>
#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_capsule_shape.h>

#include "bullet_shape.h"

namespace octoon
{
	class OCTOON_EXPORT BulletCapsuleShape : public PhysicsCapsuleShape, public BulletShape
	{
	public:
		BulletCapsuleShape(float radius, float height);
		virtual ~BulletCapsuleShape() noexcept;

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

		virtual btCollisionShape* getPxShape() const override;

	private:
		BulletCapsuleShape(const BulletCapsuleShape&) noexcept = delete;
		BulletCapsuleShape& operator=(const BulletCapsuleShape&) noexcept = delete;

	private:
		float radius_;
		float height_;

		std::unique_ptr<btCapsuleShape> shape_;
	};
}

#endif