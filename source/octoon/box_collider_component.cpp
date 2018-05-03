#include <octoon/box_collider_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/runtime/except.h>
#include <PxPhysicsAPI.h>

namespace octoon
{
    OctoonImplementSubClass(BoxCollider, Collider, "BoxCollider")

    BoxCollider::BoxCollider() noexcept
        :isRegistered(false), size(1, 1, 1)
    {
    }

	BoxCollider::BoxCollider(const math::Vector3& s) noexcept
		: isRegistered(false), size(s.x, s.y, s.z)
	{
	}

    BoxCollider::~BoxCollider()
    {
    }

    GameComponentPtr BoxCollider::clone() const noexcept
    {
        return std::make_shared<BoxCollider>();
    }

    void BoxCollider::setAutoTiling(bool isAutoTilling) noexcept
    {
        autoTiling = isAutoTilling;
        onCollisionChange();
    }

    bool BoxCollider::getAutoTiling() const noexcept
    {
        return autoTiling;
    }

    void BoxCollider::setEdgeRadius(float r) noexcept
    {
        edgeRadius = r;
    }

    float BoxCollider::getEdgeRadius() const noexcept
    {
        return edgeRadius;
    }

    void BoxCollider::setSize(const math::Vector3& s) noexcept
    {
        size = s;
    }

    math::Vector3 BoxCollider::getSize() const noexcept
    {
        return size;
    }

    void BoxCollider::onCollisionChange() noexcept
    {
    }

    void BoxCollider::onCollisionEnter() noexcept
    {
    }

    void BoxCollider::onCollisionExit() noexcept
    {
    }

    void BoxCollider::onCollisionStay() noexcept
    {
    }

    void BoxCollider::onAttach() except
    {
		auto collider = this->getComponent<Rigidbody>();
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		physx::PxVec3 dimensions(size.x, size.y, size.z);
		physx::PxBoxGeometry geometry(dimensions);

<<<<<<< HEAD
		shape = physics_feature->getSDK()->createShape(geometry, *this->shared_material->getMaterial(), true);
		if (!shape)
			runtime::runtime_error::create("create shape failed!");
		collider->body->attachShape(*shape);
		shape->release();
=======
		shape = physx::PxRigidActorExt::createExclusiveShape(*collider->body, geometry, *this->shared_material->getMaterial());
		if (!shape)
			runtime::runtime_error::create("create shape failed!");

		//getScene().addActor(*meshActor);
		//collider->body->attachShape(*shape);
>>>>>>> 4fbd90c135bcfb33da8c81f86affdd8db0d8891b
    }

    void BoxCollider::onDetach() noexcept
    {
    }

    void BoxCollider::onAttachComponent(const GameComponentPtr& component) except
    {
        if (component->isA<Rigidbody>())
        {
        }
    }

    void BoxCollider::onDetachComponent(const GameComponentPtr& component) noexcept
    {
    }
}