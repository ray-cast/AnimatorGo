#include "bullet_capsule_shape.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
    BulletCapsuleShape::BulletCapsuleShape(float radius, float height)
	{
		shape_ = std::make_unique<btCapsuleShape>(1.0f, 1.0f);
		shape_->setLocalScaling(btVector3(radius, height, 1));
		shape_->setUserPointer(this);
	}

	BulletCapsuleShape::~BulletCapsuleShape() noexcept
	{
	}

	void
	BulletCapsuleShape::setCenter(const math::float3& center)
	{
		this->center_ = center;
	}

	math::float3
	BulletCapsuleShape::getCenter()
	{
		return center_;
	}

	void
	BulletCapsuleShape::setQuaternion(const math::Quaternion& q)
	{
		this->rotation_ = q;
	}

	math::Quaternion
	BulletCapsuleShape::getQuaternion()
	{
		return rotation_;
	}

	void
	BulletCapsuleShape::setContactOffset(float offset)
	{
		shape_->setMargin(offset);
	}

	float
	BulletCapsuleShape::getContactOffset() const
	{
		return shape_->getMargin();
	}

	void
	BulletCapsuleShape::setRestOffset(float offset)
	{
	}

	float 
	BulletCapsuleShape::getRestOffset() const
	{
		return 0.0f; 
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
	BulletCapsuleShape::getShape() const
	{
		return shape_.get();
	}
}