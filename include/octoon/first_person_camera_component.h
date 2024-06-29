#ifndef OCTOON_FIRST_PERSON_CAMERA_COMPONENT_H_
#define OCTOON_FIRST_PERSON_CAMERA_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT FirstPersonCameraComponent final : public GameComponent
	{
		OctoonDeclareSubClass(FirstPersonCameraComponent, GameComponent)
	public:
		FirstPersonCameraComponent() noexcept;
		~FirstPersonCameraComponent() noexcept;

		void setSpeed(float speed) noexcept;
		void setGravity(float gravity) noexcept;
		void setMaxVelocityChange(float maxVelocityChange) noexcept;
		void setJumpHeight(float jumpHeight) noexcept;
		void setSensitivityX(float sensitivityX) noexcept;
		void setSensitivityY(float sensitivityY) noexcept;

		float getSpeed() const noexcept;
		float getGravity() const noexcept;
		float getMaxVelocityChange() const noexcept;
		float getJumpHeight() const noexcept;
		float getSensitivityX() const noexcept;
		float getSensitivityY() const noexcept;

		void upCamera(float speed) noexcept;
		void yawCamera(float speed) noexcept;
		void moveCamera(float speed) noexcept;
		void rotateCamera(float angle, const math::float3& axis) noexcept;
		void rotateCamera(float axisX, float axisY) noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onUpdate() noexcept override;

	private:
		float speed_;
		float accelerationSpeed_;
		float wheelSpeed_;
		float gravity_;
		float maxVelocityChange_;
		float jumpHeight_;
		float sensitivityX_;
		float sensitivityY_;
	};
}

#endif