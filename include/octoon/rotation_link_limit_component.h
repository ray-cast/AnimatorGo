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

		GameComponentPtr clone() const noexcept override;

	private:
		RotationLinkLimitComponent(const RotationLinkLimitComponent&) = delete;
		RotationLinkLimitComponent& operator=(const RotationLinkLimitComponent&) = delete;

	private:
		bool additiveUseLocal_;

		float additiveMoveRatio_;
		float additiveRotationRatio_;
	};
}

#endif