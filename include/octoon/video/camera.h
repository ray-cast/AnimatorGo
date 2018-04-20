#ifndef OCTOON_VIDEO_CAMERA_H_
#define OCTOON_VIDEO_CAMERA_H_

#include <octoon/video/render_object.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT Camera final : public RenderObject
		{
			OctoonDeclareSubClass(Camera, RenderObject)
		public:
			Camera() noexcept;
			virtual ~Camera() noexcept;

			void setAperture(float fov) noexcept;
			void setNear(float znear) noexcept;
			void setFar(float zfar) noexcept;
			void setRatio(float ratio) noexcept;
			void setOrtho(const math::float4& ortho) noexcept;
			void setClearColor(const math::float4& color) noexcept;
			void setViewport(const math::float4& viewport) noexcept;

			float getAperture() const noexcept;
			float getNear() const noexcept;
			float getFar() const noexcept;
			float getRatio() const noexcept;

			void setCameraOrder(CameraOrder order) noexcept;
			void setCameraType(CameraType type) noexcept;

			CameraOrder getCameraOrder() const noexcept;
			CameraType getCameraType() const noexcept;

			math::float3 worldToScreen(const math::float3& pos) const noexcept;
			math::float3 worldToProject(const math::float3& pos) const noexcept;
			math::float3 screenToWorld(const math::float3& pos) const noexcept;
			math::float3 screenToView(const math::float2& pos) const noexcept;

			const math::float4& getOrtho() const noexcept;
			const math::float4& getClearColor() const noexcept;
			const math::float4& getViewport() const noexcept;
			const math::float4& getPixelViewport() const noexcept;

			const math::float4x4& getView() const noexcept;
			const math::float4x4& getViewInverse() const noexcept;
			const math::float4x4& getViewProjection() const noexcept;
			const math::float4x4& getViewProjectionInverse() const noexcept;
			const math::float4x4& getProjection() const noexcept;
			const math::float4x4& getProjectionInverse() const noexcept;

		private:
			void onMoveAfter() noexcept;
			void onRenderBefore(const Camera& camera) noexcept;
			void onRenderAfter(const Camera& camera) noexcept;

		private:
			void _updateOrtho() const noexcept;
			void _updatePerspective() const noexcept;
			void _updateViewProject() const noexcept;

		private:
			float znear_;
			float zfar_;
			float ratio_;
			float aperture_;

			mutable bool needUpdateViewProject_;

			math::float4 ortho_;
			math::float4 clearColor_;
			math::float4 viewport_;
			mutable math::float4 screen_;

			CameraOrder cameraOrder_;
			CameraType cameraType_;

			mutable math::float4x4 viewProject_;
			mutable math::float4x4 viewProjectInverse_;
			mutable math::float4x4 project_;
			mutable math::float4x4 projectInverse_;
		};
	}
}

#endif