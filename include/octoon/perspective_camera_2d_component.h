#ifndef OCTOON_PERSPECTIVE_CAMERA_2D_COMPONENT_H_
#define OCTOON_PERSPECTIVE_CAMERA_2D_COMPONENT_H_

#include <octoon/video/perspective_camera_2d.h>
#include <octoon/camera_component.h>

namespace octoon
{
	class OCTOON_EXPORT PerspectiveCamera2DComponent : public CameraComponent
	{
		OctoonDeclareSubClass(PerspectiveCamera2DComponent, CameraComponent)
	public:
		PerspectiveCamera2DComponent() noexcept;
		explicit PerspectiveCamera2DComponent(float fov, float ratio = 1.0f, float znear = 0.01f, float zfar = 65535.0f) noexcept;
		virtual ~PerspectiveCamera2DComponent() noexcept;

		void setAperture(float fov) noexcept;
		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;
		void setRatio(float ratio) noexcept;

		float getAperture() const noexcept;
		float getNear() const noexcept;
		float getFar() const noexcept;
		float getRatio() const noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		PerspectiveCamera2DComponent(const PerspectiveCamera2DComponent&) = delete;
		PerspectiveCamera2DComponent& operator=(const PerspectiveCamera2DComponent&) = delete;

	private:
		video::PerspectiveCamera2D* camera_;
	};
}

#endif