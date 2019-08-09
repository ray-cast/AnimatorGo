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
		BoxColliderComponent(float width, float height, float depth, float contactOffset = 0.2f, float restOffset = 0.0f) noexcept;
		explicit BoxColliderComponent(const math::float3& size, float contactOffset = 0.2f, float restOffset = 0.0f) noexcept;
		virtual ~BoxColliderComponent();

		void setWidth(float width) noexcept;
		void setHeight(float height) noexcept;
		void setDepth(float depth) noexcept;
		void setContactOffset(float offset) noexcept override;
		void setRestOffset(float offset) noexcept override;

		float getWidth() const noexcept;
		float getHeight() const noexcept;
		float getDepth() const noexcept;
		float getRestOffset() const noexcept override;
		float getContactOffset() const noexcept override;

		void setSize(const math::float3& sz) noexcept;
		const math::float3& getSize() const noexcept;

		void setCenter(const math::float3& center) noexcept override;
		const math::float3& getCenter() const noexcept override;

		void setQuaternion(const math::Quaternion& rotation) noexcept;
		const math::Quaternion& getQuaternion() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		friend class RigidbodyComponent;
		std::shared_ptr<physics::PhysicsShape> getShape() noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

	private:
		float restOffset_;
		float contactOffset_;

		math::float3 size_;
		math::float3 center_;
		math::Quaternion rotation_;

		std::shared_ptr<physics::PhysicsBoxShape> shape_;
	};
}

#endif