#include <octoon/collider_component.h>
#include <PxPhysicsAPI.h>

namespace octoon
{
	OctoonImplementSubInterface(Collider, GameComponent, "Collider")

	Collider::Collider() noexcept
		: shared_material(std::make_shared<PhysicsMaterial>())
		, shape(nullptr)
	{
	}

	Collider::~Collider()
	{
	}

	void
	Collider::setBounciness(float b) noexcept
	{
		bounciness = b;
	}

	float
	Collider::getBounciness() const noexcept
	{
		return bounciness;
	}

	void
	Collider::setDensity(float d) noexcept
	{
		density = d;
	}

	float
	Collider::getDensity() const noexcept
	{
		return density;
	}

	void
	Collider::setFriction(float f) noexcept
	{
		friction = f;
	}

	float
	Collider::getFriction() const noexcept
	{
		return friction;
	}

	void
	Collider::onCollisionChange()
	{
	}

	void
	Collider::onCollisionEnter()
	{
	}

	void
	Collider::onCollisionExit()
	{
	}

	void
	Collider::onCollisionStay()
	{
	}

	void
	Collider::setSharedMaterial(PhysicsMaterial material) except
	{
		shared_material = std::make_shared<PhysicsMaterial>(material);
	}

	std::shared_ptr<PhysicsMaterial>
	Collider::getSharedMaterial() except
	{
		return shared_material;
	}
}