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
		float fov_;
		float filmSize_;
		float aperture_;
		float focalLength_;
		float focalDistance_;
		float zoom_;
		float znear_;
		float zfar_;

		math::float2 sensorSize_;

		mutable std::uint32_t width_;
		mutable std::uint32_t height_;

		mutable bool needUpdateViewProject_;
	};
}

#endif