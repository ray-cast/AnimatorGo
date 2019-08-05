#include <octoon/box_collider_component.h>
#include <octoon/physics_feature.h>
#include <octoon/game_scene.h>

namespace octoon
{
    OctoonImplementSubClass(BoxColliderComponent, ColliderComponent, "BoxCollider")

	BoxColliderComponent::BoxColliderComponent() noexcept
		: size_(math::float3::One)
		, center_(math::float3::Zero)
    {
    }

	BoxColliderComponent::BoxColliderComponent(const math::float3& size) noexcept
		: size_(size)
		, center_(math::float3::Zero)
	{
	}

	BoxColliderComponent::BoxColliderComponent(float x, float y, float z) noexcept
		: size_(math::float3(x, y, z))
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
		this->size_.x = width;
	}

	void 
	BoxColliderComponent::setHeight(float height) noexcept
	{
		if (shape_)
			shape_->setHeight(height);
		this->size_.y = height;
	}

	void
	BoxColliderComponent::setDepth(float depth) noexcept
	{
		if (shape_)
			shape_->setDepth(depth);
		this->size_.z = depth;
	}

	void
	BoxColliderComponent::setCenter(const math::float3& center) noexcept
	{
		if (shape_)
			shape_->setCenter(center);
		this->center_ = center;
	}

	void
	BoxColliderComponent::setSize(const math::float3& sz) noexcept
	{
		if (shape_)
		{
			shape_->setWidth(size_.x);
			shape_->setHeight(size_.y);
			shape_->setDepth(size_.z);
		}
		this->size_ = sz;
	}

	const math::float3&
	BoxColliderComponent::getSize() const noexcept
	{
		return this->size_;
	}

	float
	BoxColliderComponent::getWidth() const noexcept
	{
		return this->size_.x;
	}

	float
	BoxColliderComponent::getHeight() const noexcept
	{
		return this->size_.y;
	}

	float
	BoxColliderComponent::getDepth() const noexcept
	{
		return this->size_.z;
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
			boxDesc.width = size_.x;
			boxDesc.height = size_.y;
			boxDesc.depth = size_.z;
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