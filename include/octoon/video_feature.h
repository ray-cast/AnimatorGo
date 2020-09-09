#ifndef OCTOON_VIDEO_FEATURE_H_
#define OCTOON_VIDEO_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/camera_component.h>
#include <octoon/material/material.h>
#include <octoon/video/render_scene.h>

namespace octoon
{
	class OCTOON_EXPORT VideoFeature final : public GameFeature
	{
		OctoonDeclareSubClass(VideoFeature, GameFeature)
	public:
		VideoFeature() noexcept;
		VideoFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~VideoFeature() noexcept;

		void setMainCamera(CameraComponent* camera) noexcept;
		CameraComponent* getMainCamera() const noexcept;

		void setMainScene(const std::shared_ptr<video::RenderScene>& scene) noexcept;
		const std::shared_ptr<video::RenderScene>& getMainScene() const noexcept;

		void setRenderScene(const std::shared_ptr<video::RenderScene>& scene) noexcept;
		const std::shared_ptr<video::RenderScene>& getRenderScene() const noexcept;

		void setGlobalIllumination(bool enable) noexcept;
		bool getGlobalIllumination() const noexcept;

		void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

		void setOverrideMaterial(const std::shared_ptr<material::Material>& material) noexcept;
		std::shared_ptr<material::Material> getOverrideMaterial() const noexcept;

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onInputEvent(const std::any& data) noexcept;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

	private:
		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;

		CameraComponent* camera_;
		std::shared_ptr<video::RenderScene> mainScene_;
		std::shared_ptr<video::RenderScene> mainSceneDefault_;
		std::shared_ptr<video::RenderScene> renderScene_;
	};
}

#endif