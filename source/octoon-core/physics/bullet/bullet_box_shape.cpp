#include "bullet_box_shape.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
    BulletBoxShape::BulletBoxShape(float width, float height, float depth) noexcept
	{
		shape_ = std::make_unique<btBoxShape>(btVector3(0.5f, 0.5f, 0.5f));
		shape_->setLocalScaling(btVector3(width, height, depth));
		shape_->setUserPointer(this);
	}

	BulletBoxShape::~BulletBoxShape() noexcept
	{
	}

	void
	BulletBoxShape::setCenter(const math::float3& center)
	{
		this->center_ = center;
	}

	math::float3
	BulletBoxShape::getCenter()
	{
		return center_;
	}

	void
	BulletBoxShape::setQuaternion(const math::Quaternion& q)
	{
		this->rotation_ = q;
	}

	math::Quaternion
	BulletBoxShape::getQuaternion()
	{
		return rotation_;
	}

	void
	BulletBoxShape::setContactOffset(float offset)
	{
		shape_->setMargin(offset);
	}

	float
	BulletBoxShape::getContactOffset() const
	{
		return shape_->getMargin();
	}

	void
	BulletBoxShape::setRestOffset(float offset)
	{
	}

	float 
	BulletBoxShape::getRestOffset() const
	{
		return 0.0f;
	}

	void
	BulletBoxShape::setWidth(float width)
	{
		auto dimensions = shape_->getLocalScaling();
		dimensions.setX(width);
		shape_->setLocalScaling(dimensions);
	}

	void
	BulletBoxShape::setHeight(float height)
	{
		auto dimensions = shape_->getLocalScaling();
		dimensions.setY(height);
		shape_->setLocalScaling(dimensions);
	}

	void
	BulletBoxShape::setDepth(float depth)
	{
		auto dimensions = shape_->getLocalScaling();
		dimensions.setZ(depth);
		shape_->setLocalScaling(dimensions);
	}

	float
	BulletBoxShape::getWidth() const
	{
		return shape_->getLocalScaling().x();
	}

	float
	BulletBoxShape::getHeight() const
	{
		return shape_->getLocalScaling().y();
	}

	float
	BulletBoxShape::getDepth() const
	{
		return shape_->getLocalScaling().z();
	}

	btCollisionShape*
	BulletBoxShape::getShape() const
	{
		return shape_.get();
	}
}