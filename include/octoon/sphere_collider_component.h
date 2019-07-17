#ifndef OCTOON_SPHERE_COLLIDER_COMPONENT_H_
#define OCTOON_SPHERE_COLLIDER_COMPONENT_H_

#include <octoon/collider_component.h>
#include <octoon/physics/physics_sphere_shape.h>

namespace octoon
{
	class OCTOON_EXPORT SphereColliderComponent final : public ColliderComponent
	{
		OctoonDeclareSubClass(SphereColliderComponent, ColliderComponent)
	public:
		SphereColliderComponent() noexcept;
		SphereColliderComponent(float radius) noexcept;
		virtual ~SphereColliderComponent();

		void setRadius(float r) noexcept;
		float getRadius() const noexcept;

		GameComponentPtr clone() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onAttachComponent(const GameComponentPtr& component) except override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

	private:
		float radius_;
		std::shared_ptr<physics::PhysicsSphereShape> shape_;
	};
}

#endif