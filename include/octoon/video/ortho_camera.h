#ifndef OCTOON_VIDEO_ORTHO_CAMERA_H_
#define OCTOON_VIDEO_ORTHO_CAMERA_H_

#include <octoon/video/camera.h>

namespace octoon::video
{
	class OCTOON_EXPORT OrthoCamera final : public Camera
	{
		OctoonDeclareSubClass(OrthoCamera, Camera)
	public:
		OrthoCamera() noexcept;
		virtual ~OrthoCamera() noexcept;
		
		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;

		float getNear() const noexcept;
		float getFar() const noexcept;

		void setOrtho(const math::float4& ortho) noexcept;
		const math::float4& getOrtho() const noexcept;

		const math::float4x4& getViewProjection() const noexcept override;
		const math::float4x4& getViewProjectionInverse() const noexcept override;
		const math::float4x4& getProjection() const noexcept override;
		const math::float4x4& getProjectionInverse() const noexcept override;

	private:
		void onMoveAfter() noexcept override;

	private:
		void _updateViewProject() const noexcept;

	private:
		float zfar_;
		float znear_;

		math::float4 ortho_;

		mutable std::uint32_t width_;
		mutable std::uint32_t height_;

		mutable bool needUpdateViewProject_;
	};
}

#endif