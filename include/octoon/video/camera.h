#ifndef OCTOON_VIDEO_CAMERA_H_
#define OCTOON_VIDEO_CAMERA_H_

#include <octoon/video/render_object.h>
#include <octoon/graphics/graphics_types.h>

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

			void setClearFlags(graphics::GraphicsClearFlags clearflags) noexcept;
			void setCameraOrder(CameraOrder order) noexcept;
			void setCameraType(CameraType type) noexcept;
			void setFramebuffer(const graphics::GraphicsFramebufferPtr& framebuffer) noexcept;

			CameraOrder getCameraOrder() const noexcept;
			CameraType getCameraType() const noexcept;
			graphics::GraphicsClearFlags getClearFlags() const noexcept;
			const graphics::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

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

			void setupFramebuffers(std::uint32_t w, std::uint32_t h, graphics::GraphicsFormat foramt = graphics::GraphicsFormat::R8G8B8A8UNorm, graphics::GraphicsFormat depthStencil = graphics::GraphicsFormat::X8_D24UNormPack32) except;

		private:
			void onMoveAfter() noexcept;
			void onRenderBefore(const Camera& camera) noexcept;
			void onRenderAfter(const Camera& camera) noexcept;

		private:
			void _updateOrtho() const noexcept;
			void _updatePerspective(float ratio) const noexcept;
			void _updateViewProject() const noexcept;

		private:
			float znear_;
			float zfar_;
			float ratio_;
			float aperture_;

			math::float4 ortho_;
			math::float4 clearColor_;
			math::float4 viewport_;
			mutable math::float4 screen_;

			CameraOrder cameraOrder_;
			CameraType cameraType_;

			graphics::GraphicsClearFlags clearflags_;

			graphics::GraphicsFramebufferPtr fbo_;
			graphics::GraphicsFramebufferPtr fboMSAA_;

			graphics::GraphicsTexturePtr colorTexture_;
			graphics::GraphicsTexturePtr depthTexture_;
			graphics::GraphicsTexturePtr colorTextureMSAA_;
			graphics::GraphicsTexturePtr depthTextureMSAA_;

			mutable math::float4x4 viewProject_;
			mutable math::float4x4 viewProjectInverse_;
			mutable math::float4x4 project_;
			mutable math::float4x4 projectInverse_;

			mutable float ratioReal_;
			mutable bool needUpdateViewProject_;
		};
	}
}

#endif