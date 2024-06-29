#ifndef OCTOON_CAMERA_COMPONENT_H_
#define OCTOON_CAMERA_COMPONENT_H_

#include <octoon/camera/camera.h>
#include <octoon/game_component.h>

namespace octoon
{
	enum class CameraType
	{
		Custom,
		Main,
		UI
	};

	class OCTOON_EXPORT CameraComponent : public GameComponent, public RenderListener
	{
		OctoonDeclareSubClass(CameraComponent, GameComponent)
	public:
		CameraComponent() noexcept;
		CameraComponent(std::shared_ptr<Camera>&& camera) noexcept;
		virtual ~CameraComponent() noexcept;

		void setClearColor(const math::float4& color) noexcept;
		void setViewport(const math::float4& viewport) noexcept;
		void setClearFlags(ClearFlags clearflags) noexcept;
		void setCameraType(CameraType type) noexcept;
		void setFramebuffer(const GraphicsFramebufferPtr& framebuffer) noexcept;
		void setCullingMask(std::uint32_t mask) noexcept;

		std::uint32_t getCullingMask() const noexcept;
		CameraType getCameraType() const noexcept;
		ClearFlags getClearFlags() const noexcept;
		GraphicsFramebufferPtr getFramebuffer() const noexcept;

		math::float3 worldToScreen(const math::float3& pos) const noexcept;
		math::float3 worldToProject(const math::float3& pos) const noexcept;
		math::float3 screenToWorld(const math::float3& pos) const noexcept;
		math::float3 screenToView(const math::float2& pos) const noexcept;
		math::Raycast screenToRay(const math::float2& pos) const noexcept;

		const math::float4& getClearColor() const noexcept;
		const math::float4& getViewport() const noexcept;
		const math::float4& getPixelViewport() const noexcept;

		const math::float4x4& getView() const noexcept;
		const math::float4x4& getViewInverse() const noexcept;
		const math::float4x4& getViewProjection() const noexcept;
		const math::float4x4& getViewProjectionInverse() const noexcept;
		const math::float4x4& getProjection() const noexcept;
		const math::float4x4& getProjectionInverse() const noexcept;

		void setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, GraphicsFormat format = GraphicsFormat::R8G8B8A8UNorm, GraphicsFormat depthStencil = GraphicsFormat::X8_D24UNormPack32) noexcept(false);
		void setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, GraphicsFormat format = GraphicsFormat::R8G8B8A8UNorm, GraphicsFormat depthStencil = GraphicsFormat::X8_D24UNormPack32) noexcept(false);

	protected:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onPreRender(const Camera& camera) noexcept override;
		void onPostRender(const Camera& camera) noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		CameraComponent(const CameraComponent&) = delete;
		CameraComponent& operator=(const CameraComponent&) = delete;

	protected:
		CameraType cameraType_;
		std::shared_ptr<Camera> camera_;
	};
}

#endif