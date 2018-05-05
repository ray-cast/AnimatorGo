#include <octoon/box_collider_component.h>
#include <octoon/runtime/except.h>
#include <PxPhysicsAPI.h>

namespace octoon
{
	OctoonImplementSubClass(BoxCollider, Collider, "BoxCollider")

	BoxCollider::BoxCollider() noexcept
		: size(1, 1, 1)
	{
	}

	BoxCollider::BoxCollider(const math::Vector3& s) noexcept
		: size(s.x, s.y, s.z)
	{
	}

	BoxCollider::~BoxCollider()
	{
	}

	void
	BoxCollider::setSize(const math::Vector3& s) noexcept
	{
		size = s;
	}

	const math::Vector3&
	BoxCollider::getSize() const noexcept
	{
		return size;
	}

	GameComponentPtr
	BoxCollider::clone() const noexcept
	{
		auto instance = std::make_shared<BoxCollider>();
		instance->setName(this->getName());
		instance->setSize(this->getSize());

		return instance;
	}

	void
	BoxCollider::onAttach() except
	{
		buildCollider();
	}

	void
	BoxCollider::onDetach() noexcept
	{
		releaseCollider();
	}

	void
	BoxCollider::buildCollider() except
	{
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		physx::PxVec3 dimensions(size.x / 2, size.y / 2, size.z / 2);
		physx::PxBoxGeometry geometry(dimensions);
		shape = physics_feature->getSDK()->createShape(geometry, *this->shared_material->getMaterial()); // reference count is 1
		if (!shape)
			throw runtime::runtime_error::create("create shape failed!");
	}

	void
	BoxCollider::releaseCollider() except
	{
		shape->release();
		shape = nullptr;
	}
}