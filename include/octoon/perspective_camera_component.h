#ifndef OCTOON_PERSPECTIVE_CAMERA_COMPONENT_H_
#define OCTOON_PERSPECTIVE_CAMERA_COMPONENT_H_

#include <octoon/camera/perspective_camera.h>
#include <octoon/camera_component.h>

namespace octoon
{
	class OCTOON_EXPORT PerspectiveCameraComponent final : public CameraComponent
	{
		OctoonDeclareSubClass(PerspectiveCameraComponent, CameraComponent)
	public:
		PerspectiveCameraComponent() noexcept;
		explicit PerspectiveCameraComponent(float fov, const math::float2& sensorSize = math::float2::One, float znear = 0.01f, float zfar = 65535.0f) noexcept;
		virtual ~PerspectiveCameraComponent() noexcept;

		void setAperture(float fov) noexcept;
		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;

		float getAperture() const noexcept;
		float getNear() const noexcept;
		float getFar() const noexcept;

		void setSensorSize(const math::float2& sensorSize) noexcept;
		const math::float2& getSensorSize() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFovChange(const std::any& fov) noexcept;

	private:
		PerspectiveCameraComponent(const PerspectiveCameraComponent&) = delete;
		PerspectiveCameraComponent& operator=(const PerspectiveCameraComponent&) = delete;

	private:
		camera::PerspectiveCamera* camera_;
	};
}

#endif