#ifndef OCTOON_VIDEO_CAMERA_H_
#define OCTOON_VIDEO_CAMERA_H_

#include <octoon/video/render_object.h>
#include <octoon/graphics/graphics_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT Camera : public RenderObject
		{
			OctoonDeclareSubClass(Camera, RenderObject)
		public:
			Camera() noexcept;
			virtual ~Camera() noexcept;

			virtual void setClearColor(const math::float4& color) noexcept;
			virtual void setViewport(const math::float4& viewport) noexcept;

			virtual void setClearFlags(graphics::GraphicsClearFlags clearflags) noexcept;
			virtual void setCameraOrder(CameraOrder order) noexcept;
			virtual void setFramebuffer(const graphics::GraphicsFramebufferPtr& framebuffer) noexcept;

			virtual void setProjection(math::float4x4& projection) const noexcept;
			virtual void setProjectionInverse(math::float4x4& projection) const noexcept;

			virtual CameraOrder getCameraOrder() const noexcept;
			virtual graphics::GraphicsClearFlags getClearFlags() const noexcept;
			virtual const graphics::GraphicsFramebufferPtr& getFramebuffer() const noexcept;
			virtual const graphics::GraphicsFramebufferPtr& getSwapFramebuffer() const noexcept;

			virtual math::float3 worldToScreen(const math::float3& pos) const noexcept;
			virtual math::float3 worldToProject(const math::float3& pos) const noexcept;
			virtual math::float3 screenToWorld(const math::float3& pos) const noexcept;
			virtual math::float3 screenToView(const math::float2& pos) const noexcept;

			virtual const math::float4& getClearColor() const noexcept;
			virtual const math::float4& getViewport() const noexcept;
			virtual const math::float4& getPixelViewport() const noexcept;

			virtual const math::float4x4& getView() const noexcept;
			virtual const math::float4x4& getViewInverse() const noexcept;

			virtual const math::float4x4& getProjection() const noexcept;
			virtual const math::float4x4& getProjectionInverse() const noexcept;

			virtual const math::float4x4& getViewProjection() const noexcept;
			virtual const math::float4x4& getViewProjectionInverse() const noexcept;

			virtual void setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, graphics::GraphicsFormat format = graphics::GraphicsFormat::R8G8B8A8UNorm, graphics::GraphicsFormat depthStencil = graphics::GraphicsFormat::X8_D24UNormPack32) except;
			virtual void setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, graphics::GraphicsFormat format = graphics::GraphicsFormat::R8G8B8A8UNorm, graphics::GraphicsFormat depthStencil = graphics::GraphicsFormat::X8_D24UNormPack32) except;

		protected:
			math::float4 clearColor_;
			math::float4 viewport_;
			mutable math::float4 screen_;

			CameraOrder cameraOrder_;

			graphics::GraphicsClearFlags clearflags_;

			graphics::GraphicsFramebufferPtr fbo_[2];
			graphics::GraphicsTexturePtr colorTexture_[2];
			graphics::GraphicsTexturePtr depthTexture_[2];

			mutable math::float4x4 viewProject_;
			mutable math::float4x4 viewProjectInverse_;
			mutable math::float4x4 project_;
			mutable math::float4x4 projectInverse_;
		};
	}
}

#endif