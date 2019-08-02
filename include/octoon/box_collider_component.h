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

		std::shared_ptr<physics::PhysicsShape> getShape() noexcept override;

		GameComponentPtr clone() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

	private:
		float width_;
		float height_;
		float depth_;

		std::shared_ptr<physics::PhysicsBoxShape> shape_;
	};
}

#endif