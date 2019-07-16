#ifndef OCTOON_EDITOR_CAMERA_COMPONENT_H_
#define OCTOON_EDITOR_CAMERA_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT EditorCameraComponent final : public GameComponent
	{
		OctoonDeclareSubClass(EditorCameraComponent, GameComponent)
	public:
		EditorCameraComponent() noexcept;
		~EditorCameraComponent() noexcept;

		void setSpeed(float speed) noexcept;
		void setMoveSpeed(float speed) noexcept;
		void setSensitivityX(float sensitivityX) noexcept;
		void setSensitivityY(float sensitivityY) noexcept;

		float getSpeed() const noexcept;
		float getMoveSpeed() const noexcept;
		float getSensitivityX() const noexcept;
		float getSensitivityY() const noexcept;

		void upCamera(float speed) noexcept;
		void yawCamera(float speed) noexcept;
		void moveCamera(float speed) noexcept;
		void rotateCamera(float angle, const math::float3& axis) noexcept;
		void rotateCamera(float axisX, float axisY) noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFrame() noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		float speed_;
		float moveSpeed_;
		float sensitivityX_;
		float sensitivityY_;
	};
}

#endif