#ifndef OCTOON_COLLIDER_COMPONENT_H_
#define OCTOON_COLLIDER_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/physics/physics_shape.h>

namespace octoon
{
	class OCTOON_EXPORT ColliderComponent : public GameComponent
	{
		OctoonDeclareSubInterface(ColliderComponent, GameComponent)
	public:
		ColliderComponent() noexcept;
		virtual ~ColliderComponent() noexcept;

		virtual std::shared_ptr<physics::PhysicsShape> getShape() noexcept = 0;

	protected:
		virtual void OnCollisionEnter() noexcept;
		virtual void OnCollisionExit() noexcept;
		virtual void OnCollisionStay() noexcept;
	};
}

#endif