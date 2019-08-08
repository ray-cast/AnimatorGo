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
		explicit SphereColliderComponent(float radius, float contactOffset = 0.2f, float restOffset = 0.0f) noexcept;
		virtual ~SphereColliderComponent();

		void setRadius(float r) noexcept;
		float getRadius() const noexcept;

		void setCenter(const math::float3& center) noexcept override;
		const math::float3& getCenter() const noexcept override;

		void setContactOffset(float offset) noexcept override;
		float getContactOffset() const noexcept override;

		void setRestOffset(float offset) noexcept override;
		float getRestOffset() const noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		friend class RigidbodyComponent;
		std::shared_ptr<physics::PhysicsShape> getShape() noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

	private:
		float radius_;
		float restOffset_;
		float contactOffset_;

		math::float3 center_;

		std::shared_ptr<physics::PhysicsSphereShape> shape_;
	};
}

#endif