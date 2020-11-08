#include "bullet_capsule_shape.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
    namespace physics
    {
        BulletCapsuleShape::BulletCapsuleShape(float radius, float height)
		{
			shape_ = std::make_unique<btCapsuleShape>(1, 1);
			shape_->setUserPointer(this);
		}

		BulletCapsuleShape::~BulletCapsuleShape() noexcept
		{
		}

		math::float3
		BulletCapsuleShape::getCenter()
		{
			return math::float3::Zero;
		}

		void
		BulletCapsuleShape::setCenter(const math::float3& center)
		{
		}

		math::Quaternion
		BulletCapsuleShape::getQuaternion()
		{
			return math::Quaternion::Zero;
		}

		void
		BulletCapsuleShape::setQuaternion(const math::Quaternion& q)
		{
		}

		void
		BulletCapsuleShape::setContactOffset(float offset)
		{
		}

		float
		BulletCapsuleShape::getContactOffset() const
		{
			return 0.0f;
		}

		void
		BulletCapsuleShape::setRestOffset(float offset)
		{
			shape_->setMargin(offset);
		}

		float 
		BulletCapsuleShape::getRestOffset() const
		{
			return shape_->getMargin();
		}

		void
		BulletCapsuleShape::setRadius(float radius)
		{
			shape_->setLocalScaling(btVector3(radius, shape_->getHalfHeight(), 1));
		}

		void
		BulletCapsuleShape::setHeight(float height)
		{
			shape_->setLocalScaling(btVector3(shape_->getRadius(), height, 1));
		}
	
		float
		BulletCapsuleShape::getRadius() const
		{
			return shape_->getRadius();
		}

		float
		BulletCapsuleShape::getHeight() const
		{
			return shape_->getHalfHeight();
		}

		btCollisionShape*
		BulletCapsuleShape::getPxShape() const
		{
			return shape_.get();
		}
    }
}