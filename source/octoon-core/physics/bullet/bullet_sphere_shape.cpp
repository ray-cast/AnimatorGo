#include "bullet_sphere_shape.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
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
		shape_->setMargin(offset);
	}

	float
	BulletSphereShape::getContactOffset() const
	{
		return shape_->getMargin();
	}

	void
	BulletSphereShape::setRestOffset(float offset)
	{
	}

	float 
	BulletSphereShape::getRestOffset() const
	{
		return 0.0f;
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
	BulletSphereShape::getShape() const
	{
		return shape_.get();
	}
}