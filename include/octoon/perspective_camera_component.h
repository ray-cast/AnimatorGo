#ifndef OCTOON_PERSPECTIVE_CAMERA_COMPONENT_H_
#define OCTOON_PERSPECTIVE_CAMERA_COMPONENT_H_

#include <octoon/video/perspective_camera.h>
#include <octoon/camera_component.h>

namespace octoon
{
	class OCTOON_EXPORT PerspectiveCameraComponent final : public CameraComponent
	{
		OctoonDeclareSubClass(PerspectiveCameraComponent, CameraComponent)
	public:
		PerspectiveCameraComponent() noexcept;
		explicit PerspectiveCameraComponent(float fov, float ratio = 1.0f, float znear = 0.01f, float zfar = 65535.0f) noexcept;
		virtual ~PerspectiveCameraComponent() noexcept;

		void setAperture(float fov) noexcept;
		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;
		void setRatio(float ratio) noexcept;

		float getAperture() const noexcept;
		float getNear() const noexcept;
		float getFar() const noexcept;
		float getRatio() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		PerspectiveCameraComponent(const PerspectiveCameraComponent&) = delete;
		PerspectiveCameraComponent& operator=(const PerspectiveCameraComponent&) = delete;

	private:
		video::PerspectiveCamera* camera_;
	};
}

#endif