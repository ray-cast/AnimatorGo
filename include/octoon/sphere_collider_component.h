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

		void setCenter(const math::float3& center) noexcept override;
		const math::float3& getCenter() const noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		friend class RigidbodyComponent;
		std::shared_ptr<physics::PhysicsShape> getShape() noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

	private:
		float radius_;
		math::float3 center_;
		std::shared_ptr<physics::PhysicsSphereShape> shape_;
	};
}

#endif