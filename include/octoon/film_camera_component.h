#ifndef OCTOON_FILM_CAMERA_COMPONENT_H_
#define OCTOON_FILM_CAMERA_COMPONENT_H_

#include <octoon/video/film_camera.h>
#include <octoon/camera_component.h>

namespace octoon
{
	class OCTOON_EXPORT FilmCameraComponent final : public CameraComponent
	{
		OctoonDeclareSubClass(FilmCameraComponent, CameraComponent)
	public:
		FilmCameraComponent() noexcept;
		virtual ~FilmCameraComponent() noexcept;

		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;
		void setRatio(float ratio) noexcept;
		void setZoom(float zoom) noexcept;
		void setAperture(float fov) noexcept;
		void setFilmSize(float width) noexcept;
		void setFocalLength(float length) noexcept;
		void setCanvasWidth(float width) noexcept;

		float getNear() const noexcept;
		float getFar() const noexcept;
		float getRatio() const noexcept;
		float getZoom() const noexcept;
		float getAperture() const noexcept;
		float getFilmSize() const noexcept;
		float getFocalLength() const noexcept;
		float getCanvasWidth() const noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		FilmCameraComponent(const FilmCameraComponent&) = delete;
		FilmCameraComponent& operator=(const FilmCameraComponent&) = delete;

	private:
		video::FilmCamera* camera_;
	};
}

#endif