#ifndef OCTOON_BONE_CONTROLLER_COMPONENT_H_
#define OCTOON_BONE_CONTROLLER_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT RotationLinkComponent final : public GameComponent
	{
		OctoonDeclareSubClass(RotationLinkComponent, GameComponent)
	public:
		RotationLinkComponent() noexcept;

		void setAdditiveUseLocal(bool enable) noexcept;
		void setAdditiveMoveRatio(float ratio) noexcept;
		void setAdditiveRotationRatio(float ratio) noexcept;

		bool getAdditiveUseLocal() const noexcept;
		float getAdditiveMoveRatio() const noexcept;
		float getAdditiveRotationRatio() const noexcept;

		void addBone(GameObjectPtr&& bone) noexcept;
		void addBone(const GameObjectPtr& bone) noexcept;

		void setBones(GameObjects&& joint) noexcept;
		void setBones(const GameObjects& joint) noexcept;
		const GameObjects& getBones() const noexcept;

		GameComponentPtr clone() const noexcept override;

		void solve() noexcept;

	private:
		void onActivate() noexcept override;

	private:
		math::float3 deltaTranslate(bool useLocal) noexcept;
		math::Quaternion deltaRotation(bool useLocal) noexcept;

	private:
		RotationLinkComponent(const RotationLinkComponent&) = delete;
		RotationLinkComponent& operator=(const RotationLinkComponent&) = delete;

	private:
		bool additiveUseLocal_;

		float additiveMoveRatio_;
		float additiveRotationRatio_;

		math::float3 translate_;
		math::float3 localTranslate_;

		math::Quaternion rotation_;
		math::Quaternion localRotation_;

		GameObjects bones_;
	};
}

#endif