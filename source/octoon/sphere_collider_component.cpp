#include <octoon/sphere_collider_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/runtime/except.h>
#include <PxPhysicsAPI.h>


namespace octoon
{
    OctoonImplementSubClass(SphereCollider, Collider, "SphereCollider")

    SphereCollider::SphereCollider() noexcept
        :isRegistered(false), radius(1.f)
    {

    }

	SphereCollider::SphereCollider(float r) noexcept
		: isRegistered(false), radius(r)
	{

	}

    SphereCollider::~SphereCollider()
    {

    }

    GameComponentPtr SphereCollider::clone() const noexcept
    {
        return std::make_shared<SphereCollider>();
    }

    void SphereCollider::setRadius(float r) noexcept
    {
        radius = r;
    }

    float SphereCollider::getRadius() const noexcept
    {
        return radius;
    }

    void SphereCollider::onCollisionChange() noexcept
    {
    }

    void SphereCollider::onCollisionEnter() noexcept
    {
    }

    void SphereCollider::onCollisionExit() noexcept
    {
    }

    void SphereCollider::onCollisionStay() noexcept
    {

    }

    void SphereCollider::onAttach() except
    {
		auto collider = this->getComponent<Rigidbody>();
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		physx::PxSphereGeometry geometry(radius);

		shape = physx::PxRigidActorExt::createExclusiveShape(*collider->body, geometry, *this->shared_material->getMaterial());
		if (!shape)
			runtime::runtime_error::create("create shape failed!");
    }

    void SphereCollider::onDetach() noexcept
    {
    }

    void SphereCollider::onAttachComponent(const GameComponentPtr& component) except
    {
    }

    void SphereCollider::onDetachComponent(const GameComponentPtr& component) noexcept
    {
    }
}