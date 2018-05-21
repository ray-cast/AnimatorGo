#ifndef OCTOON_COLLIDER_COMPONENT_H_
#define OCTOON_COLLIDER_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/math/math.h>
#include <octoon/physics_material.h>

namespace physx
{
	class PxShape;
}

namespace octoon
{
	class OCTOON_EXPORT Collider : public GameComponent
	{
		OctoonDeclareSubInterface(Collider, GameComponent)
	public:
		Collider() noexcept;
		~Collider();

		void setSharedMaterial(PhysicsMaterial material) except;
		std::shared_ptr<PhysicsMaterial> getSharedMaterial() except;

		physx::PxShape* getShape() noexcept { return shape; }

	protected:
		virtual void onCollisionChange();
		virtual void onCollisionEnter();
		virtual void onCollisionExit();
		virtual void onCollisionStay();

	protected:
		physx::PxShape* shape;

		std::shared_ptr<PhysicsMaterial> shared_material; // The PhysicsMaterial that is applied to this collider.

		friend class Rigidbody;
	};
}

#endif // OCTOON_COLLIDER_COMPONENT_H_