#ifndef OCTOON_FIRST_PERSON_CAMERA_H_
#define OCTOON_FIRST_PERSON_CAMERA_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT FirstPersonCameraComponent : public GameComponent
	{
		OctoonDeclareSubClass(FirstPersonCameraComponent, GameComponent)
	public:
		FirstPersonCameraComponent() noexcept;
		~FirstPersonCameraComponent() noexcept;

		void upCamera(float speed) noexcept;
		void yawCamera(float speed) noexcept;
		void moveCamera(float speed) noexcept;
		void rotateCamera(float angle, const math::float3& axis) noexcept;
		void rotateCamera(float axisX, float axisY) noexcept;

	private:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onFrame() noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		float _speed;
		float _gravity;
		float _maxVelocityChange;
		float _jumpHeight;

		float _sensitivityX;
		float _sensitivityY;
	};
}

#endif