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

		virtual void setCenter(const math::float3& center) noexcept = 0;
		virtual const math::float3& getCenter() const noexcept = 0;
	
	private:
		friend class RigidbodyComponent;
		virtual std::shared_ptr<physics::PhysicsShape> getShape() noexcept = 0;
	};
}

#endif