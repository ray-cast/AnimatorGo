#ifndef OCTOON_ORTHOGRAPHIC_CAMERA_H_
#define OCTOON_ORTHOGRAPHIC_CAMERA_H_

#include <octoon/camera/camera.h>

namespace octoon::camera
{
	class OCTOON_EXPORT OrthographicCamera final : public Camera
	{
		OctoonDeclareSubClass(OrthographicCamera, Camera)
	public:
		OrthographicCamera() noexcept;
		OrthographicCamera(float left, float right, float bottom, float top, float znear, float zfar) noexcept;
		virtual ~OrthographicCamera() noexcept;
		
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