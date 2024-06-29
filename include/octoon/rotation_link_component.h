#ifndef OCTOON_ROTATION_LINK_COMPONENT_H_
#define OCTOON_ROTATION_LINK_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT RotationLinkComponent final : public GameComponent
	{
		OctoonDeclareSubClass(RotationLinkComponent, GameComponent)
	public:
		RotationLinkComponent() noexcept;
		RotationLinkComponent(const GameObjects& bones) noexcept;
		RotationLinkComponent(const GameObjectPtr& bones) noexcept;

		void addBone(GameObjectPtr&& bone) noexcept;
		void addBone(const GameObjectPtr& bone) noexcept;

		void setBones(GameObjects&& bones) noexcept;
		void setBones(const GameObjects& bones) noexcept;
		const GameObjects& getBones() const noexcept;
		
		void setTranslate(const math::float3& translate) noexcept;
		void setRotation(const math::Quaternion& quat) noexcept;

		void setLocalTranslate(const math::float3& translate) noexcept;
		void setLocalQuaternion(const math::Quaternion& quat) noexcept;

		const math::float3& getTranslate() const noexcept;
		const math::float3& getLocalTranslate() const noexcept;

		const math::Quaternion& getRotation() const noexcept;
		const math::Quaternion& getLocalQuaternion() const noexcept;

		math::float3 getDeltaTranslate(bool useLocal) const noexcept;
		math::Quaternion getDeltaRotation(bool useLocal) const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		RotationLinkComponent(const RotationLinkComponent&) = delete;
		RotationLinkComponent& operator=(const RotationLinkComponent&) = delete;

	private:
		math::float3 translate_;
		math::float3 localTranslate_;

		math::Quaternion rotation_;
		math::Quaternion localRotation_;

		GameObjects bones_;
	};
}

#endif