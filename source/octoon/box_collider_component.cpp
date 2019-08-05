#include <octoon/box_collider_component.h>
#include <octoon/physics_feature.h>
#include <octoon/game_scene.h>

namespace octoon
{
    OctoonImplementSubClass(BoxColliderComponent, ColliderComponent, "BoxCollider")

	BoxColliderComponent::BoxColliderComponent() noexcept
		: width_(1.0)
		, height_(1.0)
		, depth_(1.0)
		, center_(math::float3::Zero)
    {
    }

	BoxColliderComponent::BoxColliderComponent(const math::float3& size) noexcept
		: width_(size.x)
		, height_(size.y)
		, depth_(size.z)
		, center_(math::float3::Zero)
	{
	}

	BoxColliderComponent::BoxColliderComponent(float x, float y, float z) noexcept
		: width_(x)
		, height_(y)
		, depth_(z)
		, center_(math::float3::Zero)
	{
	}

    BoxColliderComponent::~BoxColliderComponent()
    {
    }

	void
	BoxColliderComponent::setWidth(float width) noexcept
	{
		if (shape_)
			shape_->setWidth(width);
		this->width_ = width;
	}

	void 
	BoxColliderComponent::setHeight(float height) noexcept
	{
		if (shape_)
			shape_->setHeight(height);
		this->height_ = height;
	}

	void
	BoxColliderComponent::setDepth(float depth) noexcept
	{
		if (shape_)
			shape_->setDepth(depth);
		this->depth_ = depth;
	}

	void
	BoxColliderComponent::setCenter(const math::float3& center) noexcept
	{
		if (shape_)
			shape_->setCenter(center);
		this->center_ = center;
	}

	float
	BoxColliderComponent::getWidth() const noexcept
	{
		return this->width_;
	}

	float
	BoxColliderComponent::getHeight() const noexcept
	{
		return this->height_;
	}

	float
	BoxColliderComponent::getDepth() const noexcept
	{
		return this->depth_;
	}

	const math::float3& 
	BoxColliderComponent::getCenter() const noexcept
	{
		return this->center_;
	}

    GameComponentPtr
	BoxColliderComponent::clone() const noexcept
    {
		auto instance = std::make_shared<BoxColliderComponent>();
		instance->setName(this->getName());
		instance->setWidth(this->getWidth());
		instance->setHeight(this->getHeight());
		instance->setDepth(this->getDepth());
		instance->setCenter(this->getCenter());
		return instance;
    }

	std::shared_ptr<physics::PhysicsShape>
	BoxColliderComponent::getShape() noexcept
	{
		return this->shape_;
	}

    void
	BoxColliderComponent::onActivate() except
    {
		auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
		if (physicsFeature)
		{
			physics::PhysicsBoxShapeDesc boxDesc;
			boxDesc.width = width_;
			boxDesc.height = height_;
			boxDesc.depth = depth_;
			shape_ = physicsFeature->getContext()->createBoxShape(boxDesc);
			shape_->setCenter(this->getCenter());
		}
    }

    void
	BoxColliderComponent::onDeactivate() noexcept
    {
		shape_.reset();
		shape_ = nullptr;
    }
}