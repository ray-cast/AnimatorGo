#ifndef OCTOON_VIDEO_FORWARD_RENDERER_H_
#define OCTOON_VIDEO_FORWARD_RENDERER_H_

#include <octoon/video/render_scene.h>
#include <octoon/video/scriptable_render_context.h>

#include "forward_output.h"
#include "forward_pipeline.h"
#include "forward_scene_controller.h"

namespace octoon
{
	class OCTOON_EXPORT ForwardRenderer final
	{
	public:
		ForwardRenderer() noexcept;
		virtual ~ForwardRenderer() noexcept;

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

		void render(const std::shared_ptr<ScriptableRenderContext>& context, const std::shared_ptr<RenderScene>& scene) noexcept;

	private:
		ForwardRenderer(const ForwardRenderer&) = delete;
		ForwardRenderer& operator=(const ForwardRenderer&) = delete;

	private:
		ForwardScene profile_;
		std::unique_ptr<ForwardPipeline> pipeline_;
		std::unique_ptr<ForwardSceneController> controller_;
	};
}

#endif