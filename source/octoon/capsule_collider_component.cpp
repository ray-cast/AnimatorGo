#include <octoon/capsule_collider_component.h>
#include <octoon/physics_feature.h>
#include <octoon/game_scene.h>

namespace octoon
{
    OctoonImplementSubClass(CapsuleColliderComponent, ColliderComponent, "CapsuleCollider")

	CapsuleColliderComponent::CapsuleColliderComponent() noexcept
		: radius_(1.0)
		, height_(1.0)
		, center_(math::float3::Zero)
		, contactOffset_(0.2f)
		, restOffset_(0.0f)
    {
    }

	CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height, float contactOffset, float restOffset) noexcept
		: radius_(radius)
		, height_(height)
		, center_(math::float3::Zero)
		, contactOffset_(contactOffset)
		, restOffset_(restOffset)
	{
		assert(contactOffset > restOffset);
	}

    CapsuleColliderComponent::~CapsuleColliderComponent()
    {
    }

	void
	CapsuleColliderComponent::setRadius(float radius) noexcept
	{
		if (shape_)
			shape_->setRadius(radius);
		this->radius_ = radius;
	}

	void 
	CapsuleColliderComponent::setHeight(float height) noexcept
	{
		if (shape_)
			shape_->setHeight(height);
		this->height_ = height;
	}
	
	void
	CapsuleColliderComponent::setCenter(const math::float3& center) noexcept
	{
		if (shape_)
			shape_->setCenter(center);
		this->center_ = center;
	}

	float
	CapsuleColliderComponent::getRadius() const noexcept
	{
		return this->radius_;
	}

	float
	CapsuleColliderComponent::getHeight() const noexcept
	{
		return this->height_;
	}

	const math::float3& 
	CapsuleColliderComponent::getCenter() const noexcept
	{
		return this->center_;
	}

	void
	CapsuleColliderComponent::setContactOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setContactOffset(offset);
		this->contactOffset_ = offset;
	}

	float
	CapsuleColliderComponent::getContactOffset() const noexcept
	{
		return this->contactOffset_;
	}

	void
	CapsuleColliderComponent::setRestOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setRestOffset(offset);
		this->restOffset_ = offset;
	}

	float
	CapsuleColliderComponent::getRestOffset() const noexcept
	{
		return this->restOffset_;
	}

    GameComponentPtr
	CapsuleColliderComponent::clone() const noexcept
    {
		auto instance = std::make_shared<CapsuleColliderComponent>();
		instance->setName(this->getName());
		instance->setRadius(this->getRadius());
		instance->setHeight(this->getHeight());
		instance->setCenter(this->getCenter());
		return instance;
    }

	std::shared_ptr<physics::PhysicsShape>
	CapsuleColliderComponent::getShape() noexcept
	{
		return this->shape_;
	}

    void
	CapsuleColliderComponent::onActivate() noexcept
    {
		auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
		if (physicsFeature)
		{
			physics::PhysicsCapsuleShapeDesc boxDesc;
			boxDesc.radius = radius_;
			boxDesc.height = height_;
			shape_ = physicsFeature->getContext()->createCapsuleShape(boxDesc);
			shape_->setCenter(this->center_);
			shape_->setContactOffset(this->contactOffset_);
			shape_->setRestOffset(this->restOffset_);
		}
    }

    void
	CapsuleColliderComponent::onDeactivate() noexcept
    {
		shape_.reset();
		shape_ = nullptr;
    }
}