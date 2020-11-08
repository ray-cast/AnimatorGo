#include "bullet_sphere_shape.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
    namespace physics
    {
        BulletSphereShape::BulletSphereShape(float radius)
		{
			shape_ = std::make_unique<btSphereShape>(radius);
			shape_->setUserPointer(this);
		}

		BulletSphereShape::~BulletSphereShape() noexcept
		{
			
		}

		void
		BulletSphereShape::setCenter(const math::float3& center)
		{
		}

		math::float3
		BulletSphereShape::getCenter()
		{
			return math::float3::Zero;
		}

		void
		BulletSphereShape::setQuaternion(const math::Quaternion& q)
		{
		}

		math::Quaternion
		BulletSphereShape::getQuaternion()
		{
			return math::Quaternion::Zero;
		}

		void
		BulletSphereShape::setContactOffset(float offset)
		{
		}

		float
		BulletSphereShape::getContactOffset() const
		{
			return 0.0f;
		}

		void
		BulletSphereShape::setRestOffset(float offset)
		{
			shape_->setMargin(offset);
		}

		float 
		BulletSphereShape::getRestOffset() const
		{
			return shape_->getMargin();
		}

		float
		BulletSphereShape::getRadius()
		{
			return shape_->getRadius();
		}

		void
		BulletSphereShape::setRadius(float radius)
		{
			shape_->setUnscaledRadius(radius);
		}

		btCollisionShape*
		BulletSphereShape::getPxShape() const
		{
			return shape_.get();
		}
    }
}