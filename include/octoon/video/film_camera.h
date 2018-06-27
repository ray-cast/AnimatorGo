#ifndef OCTOON_VIDEO_PERSPECTIVE_CAMERA_2D_H_
#define OCTOON_VIDEO_PERSPECTIVE_CAMERA_2D_H_

#include <octoon/video/camera.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT FilmCamera final : public Camera
		{
			OctoonDeclareSubClass(FilmCamera, Camera)
		public:
			FilmCamera() noexcept;
			virtual ~FilmCamera() noexcept;

			void setNear(float znear) noexcept;
			void setFar(float zfar) noexcept;
			void setRatio(float ratio) noexcept;

			float getNear() const noexcept;
			float getFar() const noexcept;
			float getRatio() const noexcept;

			void setZoom(float zoom) noexcept;
			void setAperture(float fov) noexcept;
			void setFilmSize(float width) noexcept;
			void setFocalLength(float length) noexcept;

			float getZoom() const noexcept;
			float getAperture() const noexcept;
			float getFilmSize() const noexcept;
			float getFocalLength() const noexcept;

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
			float ratio_;
			float aperture_;
			float zoom_;
			float filmSize_;
			float focalLength_;

			mutable std::uint32_t width_;
			mutable std::uint32_t height_;

			mutable bool needUpdateViewProject_;
		};
	}
}

#endif