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
    {
    }

	SphereColliderComponent::SphereColliderComponent(float radius) noexcept
		: radius_(radius)
	{
	}

    SphereColliderComponent::~SphereColliderComponent()
    {

    }

    GameComponentPtr
	SphereColliderComponent::clone() const noexcept
    {
        return std::make_shared<SphereColliderComponent>();
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
	SphereColliderComponent::onActivate() except
    {
		auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
		auto physicsContext = physicsFeature->getContext();
		physics::PhysicsSphereShapeDesc sphereDesc;
		sphereDesc.radius = radius_;
		shape_ = physicsContext->createShape(sphereDesc);
    }

    void
	SphereColliderComponent::onDeactivate() noexcept
    {
		shape_.reset();
		shape_ = nullptr;
    }

    void
	SphereColliderComponent::onAttachComponent(const GameComponentPtr& component) except
    {
		/*if (!shape_) return;

		if (component->isA<RigidbodyComponent>())
		{
			auto rigidbody = component->downcast_pointer<RigidbodyComponent>();
			if (rigidbody)
				rigidbody->getRigidbody()->attachShape(shape_);
		}*/
    }

    void 
	SphereColliderComponent::onDetachComponent(const GameComponentPtr& component) noexcept
    {
		/*if (!shape_) return;

		if (component->isA<RigidbodyComponent>())
		{
			auto rigidbody = component->downcast_pointer<RigidbodyComponent>();
			if (rigidbody)
				rigidbody->getRigidbody()->detachShape(); // decrements reference count
		}*/
    }
}