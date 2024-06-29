#ifndef OCTOON_CAPSULE_COLLIDER_COMPONENT_H_
#define OCTOON_CAPSULE_COLLIDER_COMPONENT_H_

#include <octoon/collider_component.h>
#include <octoon/physics/physics_capsule_shape.h>

namespace octoon
{
	class OCTOON_EXPORT CapsuleColliderComponent final : public ColliderComponent
	{
		OctoonDeclareSubClass(CapsuleColliderComponent, ColliderComponent)
	public:
		CapsuleColliderComponent() noexcept;
		CapsuleColliderComponent(float radius, float height) noexcept;
		CapsuleColliderComponent(float radius, float height, float contactOffset, float restOffset) noexcept;
		virtual ~CapsuleColliderComponent();

		void setRadius(float radius) noexcept;
		void setHeight(float height) noexcept;
		void setRestOffset(float offset) noexcept override;
		void setContactOffset(float offset) noexcept override;

		float getRadius() const noexcept;
		float getHeight() const noexcept;
		float getRestOffset() const noexcept override;
		float getContactOffset() const noexcept override;

		void setCenter(const math::float3& center) noexcept override;
		const math::float3& getCenter() const noexcept override;

		void setRotation(const math::Quaternion& rotation) noexcept override;
		const math::Quaternion& getRotation() const noexcept override;

		math::float4x4 getLocalPose() const noexcept override;

		void load(const nlohmann::json& json) noexcept(false) override;
		void save(nlohmann::json& json) const noexcept(false) override;

		GameComponentPtr clone() const noexcept;

	private:
		friend class RigidbodyComponent;
		std::shared_ptr<PhysicsShape> getShape() noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

	private:
		float radius_;
		float height_;

		float restOffset_;
		float contactOffset_;

		math::float3 center_;
		math::Quaternion rotation_;

		std::shared_ptr<PhysicsCapsuleShape> shape_;
	};
}

#endif