#ifndef OCTOON_COLLIDER_COMPONENT_H_
#define OCTOON_COLLIDER_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/math/math.h>


namespace octoon
{
	class OCTOON_EXPORT ColliderComponent : public GameComponent
	{
		OctoonDeclareSubInterface(ColliderComponent, GameComponent)
	public:
		ColliderComponent() noexcept;
		~ColliderComponent();

		//void setSharedMaterial(PhysicsMaterial material) except;
		//std::shared_ptr<PhysicsMaterial> getSharedMaterial() except;

		//physx::PxShape* getShape() noexcept { return shape; }

	protected:
		//virtual void OnCollisionEnter(Collision collision); // OnCollisionEnter is called when this collider / rigidbody has begun touching another rigidbody / collider.
		//virtual void OnCollisionExit(Collision collision); // OnCollisionExit is called when this collider / rigidbody has stopped touching another rigidbody / collider.
		//virtual void OnCollisionStay(Collision collision); // OnCollisionStay is called once per frame for every collider / rigidbody that is touching rigidbody / collider.

	protected:
		//physx::PxShape* shape;

		//std::shared_ptr<PhysicsMaterial> shared_material; // The PhysicsMaterial that is applied to this collider.
	};
}

#endif // OCTOON_COLLIDER_COMPONENT_H_