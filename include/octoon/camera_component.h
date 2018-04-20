#ifndef OCTOON_CAMERA_COMPONENT_H_
#define OCTOON_CAMERA_COMPONENT_H_

#include <octoon/video/camera.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT CameraComponent final : public GameComponent, public video::RenderListener
	{
		OctoonDeclareSubClass(CameraComponent, GameComponent)
	public:
		CameraComponent() noexcept;
		virtual ~CameraComponent() noexcept;

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

		void setCameraOrder(video::CameraOrder order) noexcept;
		void setCameraType(video::CameraType type) noexcept;

		video::CameraOrder getCameraOrder() const noexcept;
		video::CameraType getCameraType() const noexcept;

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

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveBefore() noexcept override;
		virtual void onMoveAfter() noexcept override;

		virtual void onRenderObjectPre(const video::Camera& camera) noexcept override;
		virtual void onRenderObjectPost(const video::Camera& camera) noexcept override;

	private:
		CameraComponent(const CameraComponent&) = delete;
		CameraComponent& operator=(const CameraComponent&) = delete;

	private:
		video::CameraPtr camera_;
	};
}

#endif