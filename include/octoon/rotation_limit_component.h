#ifndef OCTOON_ROTATION_LIMIT_COMPONENT_H_
#define OCTOON_ROTATION_LIMIT_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT RotationLimitComponent final : public GameComponent
	{
		OctoonDeclareSubClass(RotationLimitComponent, GameComponent)
	public:
		RotationLimitComponent() noexcept;
		RotationLimitComponent(float mininumAngle, float maximumAngle, math::float3 minimumRadians, math::float3 maximumRadians) noexcept;

		void setMininumAngle(float limit) noexcept;
		void setMaximumAngle(float limit) noexcept;
		void setMinimumAxis(const math::float3& limit) noexcept;
		void setMaximumAxis(const math::float3& limit) noexcept;
		
		float getMininumAngle() const noexcept;
		float getMaximumAngle() const noexcept;

		const math::float3& getMinimumAxis() const noexcept;
		const math::float3& getMaximumAxis() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		RotationLimitComponent(const RotationLimitComponent&) = delete;
		RotationLimitComponent& operator=(const RotationLimitComponent&) = delete;

	private:
		float mininumAngle_;
		float maximumAngle_;

		math::float3 minimumAxis_;
		math::float3 maximumAxis_;
	};
}

#endif