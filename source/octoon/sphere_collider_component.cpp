#include <octoon/sphere_collider_component.h>


#include <octoon/rigidbody_component.h>
#include <octoon/physics_feature.h>
#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/except.h>


namespace octoon
{
    OctoonImplementSubClass(SphereColliderComponent, ColliderComponent, "SphereColliderComponent")

	SphereColliderComponent::SphereColliderComponent() noexcept
    {

    }

    SphereColliderComponent::~SphereColliderComponent()
    {

    }

    GameComponentPtr SphereColliderComponent::clone() const noexcept
    {
        return std::make_shared<SphereColliderComponent>();
    }

    void SphereColliderComponent::setRadius(float r) noexcept
    {
		if (shape) shape->setRadius(r);
    }

    float SphereColliderComponent::getRadius() const noexcept
    {
		if (shape)
			return shape->getRadius();
		else
			return 0.f;
    }

    void SphereColliderComponent::onAttach() except
    {
		auto physicsFeature = this->getGameObject()->getGameScene()->getFeature<PhysicsFeature>();
		auto physicsContext = physicsFeature->getContext();
		physics::PhysicsSphereShapeDesc sphereDesc;
		shape = physicsContext->createShape(sphereDesc);
    }

    void SphereColliderComponent::onDetach() noexcept
    {
		shape.reset();
    }

    void SphereColliderComponent::onAttachComponent(const GameComponentPtr& component) except
    {
    }

    void SphereColliderComponent::onDetachComponent(const GameComponentPtr& component) noexcept
    {
    }
}