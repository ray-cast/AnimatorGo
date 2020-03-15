#ifndef OCTOON_FILM_CAMERA_H_
#define OCTOON_FILM_CAMERA_H_

#include <octoon/camera/perspective_camera.h>

namespace octoon::camera
{
	class OCTOON_EXPORT FilmCamera final : public Camera
	{
		OctoonDeclareSubClass(FilmCamera, Camera)
	public:
		FilmCamera() noexcept;
		virtual ~FilmCamera() noexcept;

		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;
		void setFilmSize(float width) noexcept;
		void setCanvasWidth(float width) noexcept;
		void setFocalLength(float length) noexcept;
		void setZoom(float zoom) noexcept;
		void setAperture(float fov) noexcept;

		float getNear() const noexcept;
		float getFar() const noexcept;
		float getZoom() const noexcept;
		float getAperture() const noexcept;
		float getFilmSize() const noexcept;
		float getCanvasWidth() const noexcept;
		float getFocalLength() const noexcept;

		void setSensorSize(const math::float2& ratio) noexcept;
		const math::float2& getSensorSize() const noexcept;

		const math::float4x4& getViewProjection() const noexcept override;
		const math::float4x4& getViewProjectionInverse() const noexcept override;
		const math::float4x4& getProjection() const noexcept override;
		const math::float4x4& getProjectionInverse() const noexcept override;

	private:
		void onMoveAfter() noexcept override;

	private:
		void _updateViewProject() const noexcept;

	private:
		float znear_;
		float zfar_;
		float filmSize_;
		float focalLength_;
		float aperture_;
		float zoom_;
		float canvasWidth_;

		math::float2 sensorSize_;

		mutable std::uint32_t width_;
		mutable std::uint32_t height_;

		mutable bool needUpdateViewProject_;
	};
}

#endif