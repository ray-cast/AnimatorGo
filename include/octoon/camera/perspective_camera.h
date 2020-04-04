#ifndef OCTOON_PERSPECTIVE_CAMERA_H_
#define OCTOON_PERSPECTIVE_CAMERA_H_

#include <octoon/camera/camera.h>

namespace octoon::camera
{
	class OCTOON_EXPORT PerspectiveCamera final : public Camera
	{
		OctoonDeclareSubClass(PerspectiveCamera, Camera)
	public:
		PerspectiveCamera() noexcept;
		PerspectiveCamera(float fov, float znear, float zfar) noexcept;
		virtual ~PerspectiveCamera() noexcept;

		void setAperture(float fov) noexcept;
		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;

		float getAperture() const noexcept;
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
		float znear_;
		float zfar_;
		float aperture_;

		math::float2 sensorSize_;

		mutable std::uint32_t width_;
		mutable std::uint32_t height_;

		mutable bool needUpdateViewProject_;
	};
}

#endif