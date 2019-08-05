#ifndef OCTOON_BOX_COLLIDER_COMPONENT_H_
#define OCTOON_BOX_COLLIDER_COMPONENT_H_

#include <octoon/collider_component.h>
#include <octoon/physics/physics_box_shape.h>

namespace octoon
{
	class OCTOON_EXPORT BoxColliderComponent final : public ColliderComponent
	{
		OctoonDeclareSubClass(BoxColliderComponent, ColliderComponent)
	public:
		BoxColliderComponent() noexcept;
		BoxColliderComponent(const math::float3& size) noexcept;
		BoxColliderComponent(float width, float height, float depth) noexcept;
		virtual ~BoxColliderComponent();

		void setWidth(float width) noexcept;
		void setHeight(float height) noexcept;
		void setDepth(float depth) noexcept;

		float getWidth() const noexcept;
		float getHeight() const noexcept;
		float getDepth() const noexcept;

		void setSize(const math::float3& sz) noexcept;
		const math::float3& getSize() const noexcept;

		void setCenter(const math::float3& center) noexcept override;
		const math::float3& getCenter() const noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		friend class RigidbodyComponent;
		std::shared_ptr<physics::PhysicsShape> getShape() noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

	private:
		math::float3 size_;
		math::float3 center_;

		std::shared_ptr<physics::PhysicsBoxShape> shape_;
	};
}

#endif