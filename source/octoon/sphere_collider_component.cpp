#include <octoon/sphere_collider_component.h>

#include <octoon/rigidbody_component.h>
#include <octoon/physics_feature.h>
#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/except.h>

namespace octoon
{
    OctoonImplementSubClass(SphereColliderComponent, ColliderComponent, "SphereCollider")

	SphereColliderComponent::SphereColliderComponent() noexcept
		: radius_(1.0)
		, center_(math::float3::Zero)
		, contactOffset_(0.2f)
		, restOffset_(0.0f)
    {
    }

	SphereColliderComponent::SphereColliderComponent(float radius, float contactOffset, float restOffset) noexcept
		: radius_(radius)
		, center_(math::float3::Zero)
		, contactOffset_(contactOffset)
		, restOffset_(restOffset)
	{
		assert(contactOffset > restOffset);
	}

    SphereColliderComponent::~SphereColliderComponent()
    {
    }

    void
	SphereColliderComponent::setRadius(float radius) noexcept
    {
		if (shape_)
			shape_->setRadius(radius);
		this->radius_ = radius;
    }

    float
	SphereColliderComponent::getRadius() const noexcept
    {
		return this->radius_;
    }

	void
	SphereColliderComponent::setCenter(const math::float3& center) noexcept
	{
		if (shape_)
			shape_->setCenter(center);
		this->center_ = center;
	}

	const math::float3& 
	SphereColliderComponent::getCenter() const noexcept
	{
		return this->center_;
	}

	void
	SphereColliderComponent::setContactOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setContactOffset(offset);
		this->contactOffset_ = offset;
	}

	float
	SphereColliderComponent::getContactOffset() const noexcept
	{
		return this->contactOffset_;
	}

	void
	SphereColliderComponent::setRestOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setRestOffset(offset);
		this->restOffset_ = offset;
	}

	float
	SphereColliderComponent::getRestOffset() const noexcept
	{
		return this->restOffset_;
	}

	std::shared_ptr<physics::PhysicsShape>
	SphereColliderComponent::getShape() noexcept
	{
		return this->shape_;
	}

	GameComponentPtr
	SphereColliderComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SphereColliderComponent>();
		instance->setName(this->getName());
		instance->setRadius(this->getRadius());
		instance->setCenter(this->getCenter());
		return instance;
	}

    void
	SphereColliderComponent::onActivate() noexcept
    {
		auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
		if (physicsFeature)
		{
			physics::PhysicsSphereShapeDesc sphereDesc;
			sphereDesc.radius = radius_;
			shape_ = physicsFeature->getContext()->createSphereShape(sphereDesc);
			shape_->setCenter(this->center_);
			shape_->setContactOffset(this->contactOffset_);
			shape_->setRestOffset(this->restOffset_);
		}
    }

    void
	SphereColliderComponent::onDeactivate() noexcept
    {
		shape_.reset();
		shape_ = nullptr;
    }
}