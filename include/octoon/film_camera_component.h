#ifndef OCTOON_FILM_CAMERA_COMPONENT_H_
#define OCTOON_FILM_CAMERA_COMPONENT_H_

#include <octoon/camera/film_camera.h>
#include <octoon/camera_component.h>

namespace octoon
{
	class OCTOON_EXPORT FilmCameraComponent final : public CameraComponent
	{
		OctoonDeclareSubClass(FilmCameraComponent, CameraComponent)
	public:
		FilmCameraComponent() noexcept;
		virtual ~FilmCameraComponent() noexcept;

		void setFov(float fov) noexcept;
		void setFilmSize(float width) noexcept;
		void setAperture(float aperture) noexcept;
		void setFocalLength(float length) noexcept;
		void setFocalDistance(float distance) noexcept;
		void setZoom(float zoom) noexcept;
		void setCanvasWidth(float width) noexcept;
		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;

		float getFov() const noexcept;
		float getFilmSize() const noexcept;
		float getAperture() const noexcept;
		float getFocalLength() const noexcept;
		float getFocalDistance() const noexcept;
		float getZoom() const noexcept;
		float getNear() const noexcept;
		float getFar() const noexcept;

		void setSensorSize(const math::float2& sensorSize) noexcept;
		const math::float2& getSensorSize() const noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFovChange(const std::any& data) noexcept;

	private:
		FilmCameraComponent(const FilmCameraComponent&) = delete;
		FilmCameraComponent& operator=(const FilmCameraComponent&) = delete;

	private:
		camera::FilmCamera* camera_;
	};
}

#endif