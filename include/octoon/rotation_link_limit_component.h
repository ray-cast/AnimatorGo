#ifndef OCTOON_ROTATION_LINK_LIMIT_COMPONENT_H_
#define OCTOON_ROTATION_LINK_LIMIT_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT RotationLinkLimitComponent final : public GameComponent
	{
		OctoonDeclareSubClass(RotationLinkLimitComponent, GameComponent)
	public:
		RotationLinkLimitComponent() noexcept;
		RotationLinkLimitComponent(float move, float rotation, bool useLocal) noexcept;

		void setAdditiveUseLocal(bool enable) noexcept;
		void setAdditiveMoveRatio(float ratio) noexcept;
		void setAdditiveRotationRatio(float ratio) noexcept;

		bool getAdditiveUseLocal() const noexcept;
		float getAdditiveMoveRatio() const noexcept;
		float getAdditiveRotationRatio() const noexcept;

		void setTranslate(const math::float3& translate) noexcept;
		void setRotation(const math::Quaternion& quat) noexcept;

		void setLocalTranslate(const math::float3& translate) noexcept;
		void setLocalQuaternion(const math::Quaternion& quat) noexcept;

		const math::float3& getTranslate() const noexcept;
		const math::float3& getLocalTranslate() const noexcept;

		const math::Quaternion& getRotation() const noexcept;
		const math::Quaternion& getLocalQuaternion() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		RotationLinkLimitComponent(const RotationLinkLimitComponent&) = delete;
		RotationLinkLimitComponent& operator=(const RotationLinkLimitComponent&) = delete;

	private:
		bool additiveUseLocal_;

		float additiveMoveRatio_;
		float additiveRotationRatio_;

		math::float3 translate_;
		math::float3 localTranslate_;

		math::Quaternion rotation_;
		math::Quaternion localRotation_;
	};
}

#endif