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
		buildCollider();
    }

    void BoxCollider::onDetach() noexcept
    {
		releaseCollider();
    }

    void BoxCollider::onAttachComponent(const GameComponentPtr& component) except
    {
    }

    void BoxCollider::onDetachComponent(const GameComponentPtr& component) noexcept
    {
    }

	void BoxCollider::buildCollider() except
	{
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		physx::PxVec3 dimensions(size.x / 2, size.y / 2, size.z / 2);
		physx::PxBoxGeometry geometry(dimensions);
		shape = physics_feature->getSDK()->createShape(geometry, *this->shared_material->getMaterial()); // reference count is 1
		if (!shape)
			runtime::runtime_error::create("create shape failed!");
	}

	void BoxCollider::releaseCollider() except
	{
		shape->release();
		shape = nullptr;
	}
}