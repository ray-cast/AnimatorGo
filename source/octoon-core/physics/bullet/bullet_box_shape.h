#ifndef OCTOON_BULLET_BOX_SHAPE_H_
#define OCTOON_BULLET_BOX_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>
#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_box_shape.h>

#include "bullet_shape.h"

namespace octoon
{
	class OCTOON_EXPORT BulletBoxShape final : public PhysicsBoxShape, public BulletShape
	{
	public:
		BulletBoxShape(float hx, float hy, float hz) noexcept;
		virtual ~BulletBoxShape() noexcept;

		virtual math::float3 getCenter() override;
		virtual void setCenter(const math::float3& center) override;

		virtual math::Quaternion getQuaternion() override;
		virtual void setQuaternion(const math::Quaternion& center) override;

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

		virtual btCollisionShape* getPxShape() const override;

	private:
		BulletBoxShape(const BulletBoxShape&) noexcept = delete;
		BulletBoxShape& operator=(const BulletBoxShape&) noexcept = delete;

	private:
		float width_;
		float height_;
		float depth_;

		std::unique_ptr<btBoxShape> shape_;
	};
}

#endif