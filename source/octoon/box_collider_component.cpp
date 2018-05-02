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
		physx::PxVec3 dimensions(size.x, size.y, size.z);
		physx::PxBoxGeometry geometry(dimensions);

		shape = physx::PxRigidActorExt::createExclusiveShape(*collider->body, geometry,
			*this->shared_material->getMaterial());
		if (!shape)
			runtime::runtime_error::create("create shape failed!");
		//getScene().addActor(*meshActor);
		//collider->body->attachShape(*shape);
    }

    void BoxCollider::onDetach() noexcept
    {
		//auto collider = this->getComponent<Rigidbody>();
		//collider->body->detachShape(*shape);
		//shape->release();
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