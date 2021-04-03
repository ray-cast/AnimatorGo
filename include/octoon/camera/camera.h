#ifndef OCTOON_CAMERA_H_
#define OCTOON_CAMERA_H_

#include <octoon/video/render_object.h>
#include <octoon/hal/graphics_types.h>

namespace octoon
{
	class OCTOON_EXPORT Camera : public RenderObject
	{
		OctoonDeclareSubClass(Camera, RenderObject)
	public:
		Camera() noexcept;
		virtual ~Camera() noexcept;

		virtual void setClearColor(const math::float4& color) noexcept;
		virtual void setViewport(const math::float4& viewport) noexcept;
		virtual void setRenderToScreen(bool enable) noexcept;

		virtual void setClearFlags(hal::GraphicsClearFlags clearflags) noexcept;
		virtual void setFramebuffer(const hal::GraphicsFramebufferPtr& framebuffer) noexcept;

		virtual void setProjection(math::float4x4& projection) const noexcept;
		virtual void setProjectionInverse(math::float4x4& projection) const noexcept;

		virtual bool getRenderToScreen() const noexcept;
		virtual hal::GraphicsClearFlags getClearFlags() const noexcept;
		virtual const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

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

		virtual void setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, hal::GraphicsFormat format = hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat depthStencil = hal::GraphicsFormat::X8_D24UNormPack32) except;

	protected:
		math::float4 clearColor_;
		math::float4 viewport_;
		mutable math::float4 screen_;

		bool blitToScreen_;

		hal::GraphicsClearFlags clearflags_;

		hal::GraphicsFramebufferPtr fbo_;
		hal::GraphicsTexturePtr colorTexture_;
		hal::GraphicsTexturePtr depthTexture_;

		mutable math::float4x4 viewProject_;
		mutable math::float4x4 viewProjectInverse_;
		mutable math::float4x4 project_;
		mutable math::float4x4 projectInverse_;
	};
}

#endif