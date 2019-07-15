#ifndef OCTOON_SPHERE_COLLIDER_COMPONENT_H_
#define OCTOON_SPHERE_COLLIDER_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_sphere_shape.h>


namespace octoon
{
	class OCTOON_EXPORT SphereColliderComponent : public ColliderComponent
	{
		OctoonDeclareSubClass(SphereColliderComponent, ColliderComponent)
	public:
		SphereColliderComponent() noexcept;
		~SphereColliderComponent();
		virtual GameComponentPtr clone() const noexcept;

		void setRadius(float r) noexcept;
		float getRadius() const noexcept;

	private:
		virtual void onAttach() except;
		virtual void onDetach() noexcept;

		virtual void onAttachComponent(const GameComponentPtr& component) except;
		virtual void onDetachComponent(const GameComponentPtr& component) noexcept;
	private:
		std::shared_ptr<physics::PhysicsSphereShape> shape;
	};
}


#endif // OCTOON_SPHERE_COLLIDER_COMPONENT_H_