#include <octoon/capsule_collider_component.h>

#include <octoon/rigidbody_component.h>
#include <octoon/physics_feature.h>
#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/except.h>

namespace octoon
{
    OctoonImplementSubClass(CapsuleColliderComponent, ColliderComponent, "CapsuleCollider")

	CapsuleColliderComponent::CapsuleColliderComponent() noexcept
		: radius_(1.0)
		, height_(1.0)
    {
    }

	CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height) noexcept
		: radius_(radius)
		, height_(height)
	{
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
	
	float
	CapsuleColliderComponent::getRadius() const noexcept
	{
		return radius_;
	}

	float
	CapsuleColliderComponent::getHeight() const noexcept
	{
		return height_;
	}

    GameComponentPtr
	CapsuleColliderComponent::clone() const noexcept
    {
		auto instance = std::make_shared<CapsuleColliderComponent>();
		instance->setName(this->getName());
		instance->setRadius(this->getRadius());
		instance->setHeight(this->getHeight());
		return instance;
    }

	std::shared_ptr<physics::PhysicsShape>
	CapsuleColliderComponent::getShape() noexcept
	{
		return this->shape_;
	}

    void
	CapsuleColliderComponent::onActivate() except
    {
		auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
		if (physicsFeature)
		{
			physics::PhysicsCapsuleShapeDesc boxDesc;
			boxDesc.radius = radius_;
			boxDesc.height = height_;
			shape_ = physicsFeature->getContext()->createCapsuleShape(boxDesc);
		}
    }

    void
	CapsuleColliderComponent::onDeactivate() noexcept
    {
		shape_.reset();
		shape_ = nullptr;
    }
}