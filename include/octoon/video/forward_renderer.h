#ifndef OCTOON_VIDEO_FORWARD_RENDERER_H_
#define OCTOON_VIDEO_FORWARD_RENDERER_H_

#include <octoon/video/render_scene.h>
#include <octoon/video/lights_shadow_caster_pass.h>
#include <octoon/video/draw_object_pass.h>
#include <octoon/video/draw_skybox_pass.h>

namespace octoon
{
	class OCTOON_EXPORT ForwardRenderer final
	{
	public:
		ForwardRenderer() noexcept;
		virtual ~ForwardRenderer() noexcept;

		void render(const std::shared_ptr<ScriptableRenderContext>& context, const RenderingData& scene);

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

	private:
		void setupFramebuffers(const std::shared_ptr<ScriptableRenderContext>& context, std::uint32_t w, std::uint32_t h) except;

	private:
		ForwardRenderer(const ForwardRenderer&) = delete;
		ForwardRenderer& operator=(const ForwardRenderer&) = delete;

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		std::unique_ptr<LightsShadowCasterPass> lightsShadowCasterPass_;
		std::unique_ptr<DrawObjectPass> drawOpaquePass_;
		std::unique_ptr<DrawObjectPass> drawTranparentPass_;
		std::unique_ptr<DrawSkyboxPass> drawSkyboxPass_;

		hal::GraphicsFramebufferPtr fbo_;
		hal::GraphicsFramebufferPtr fbo2_;
		hal::GraphicsTexturePtr colorTexture_;
		hal::GraphicsTexturePtr depthTexture_;
		hal::GraphicsTexturePtr colorTexture2_;
		hal::GraphicsTexturePtr depthTexture2_;

		std::shared_ptr<Material> overrideMaterial_;

		std::unordered_map<std::intptr_t, std::shared_ptr<hal::GraphicsTexture>> lightTextures_;
	};
}

#endif