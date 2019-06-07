#ifndef OCTOON_CAMERA_COMPONENT_H_
#define OCTOON_CAMERA_COMPONENT_H_

#include <octoon/video/camera.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT CameraComponent : public GameComponent, public video::RenderListener
	{
		OctoonDeclareSubClass(CameraComponent, GameComponent)
	public:
		CameraComponent() noexcept;
		CameraComponent(video::CameraPtr&& camera) noexcept;
		virtual ~CameraComponent() noexcept;

		void setClearColor(const math::float4& color) noexcept;
		void setViewport(const math::float4& viewport) noexcept;
		void setClearFlags(hal::GraphicsClearFlags clearflags) noexcept;
		void setCameraType(video::CameraType type) noexcept;
		void setFramebuffer(const hal::GraphicsFramebufferPtr& framebuffer) noexcept;

		video::CameraType getCameraType() const noexcept;
		hal::GraphicsClearFlags getClearFlags() const noexcept;
		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;
		const hal::GraphicsFramebufferPtr& getSwapFramebuffer() const noexcept;

		math::float3 worldToScreen(const math::float3& pos) const noexcept;
		math::float3 worldToProject(const math::float3& pos) const noexcept;
		math::float3 screenToWorld(const math::float3& pos) const noexcept;
		math::float3 screenToView(const math::float2& pos) const noexcept;

		const math::float4& getClearColor() const noexcept;
		const math::float4& getViewport() const noexcept;
		const math::float4& getPixelViewport() const noexcept;

		const math::float4x4& getView() const noexcept;
		const math::float4x4& getViewInverse() const noexcept;
		const math::float4x4& getViewProjection() const noexcept;
		const math::float4x4& getViewProjectionInverse() const noexcept;
		const math::float4x4& getProjection() const noexcept;
		const math::float4x4& getProjectionInverse() const noexcept;

		void setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, hal::GraphicsFormat format = hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat depthStencil = hal::GraphicsFormat::X8_D24UNormPack32) except;
		void setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, hal::GraphicsFormat format = hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat depthStencil = hal::GraphicsFormat::X8_D24UNormPack32) except;

	private:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveBefore() noexcept override;
		virtual void onMoveAfter() noexcept override;

		virtual void onRenderObjectPre(const video::Camera& camera) noexcept override;
		virtual void onRenderObjectPost(const video::Camera& camera) noexcept override;

		virtual void onLayerChangeAfter() noexcept override;

	private:
		CameraComponent(const CameraComponent&) = delete;
		CameraComponent& operator=(const CameraComponent&) = delete;

	protected:
		video::CameraPtr camera_;
	};
}

#endif