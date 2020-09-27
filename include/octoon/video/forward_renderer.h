#ifndef OCTOON_VIDEO_FORWARD_RENDERER_H_
#define OCTOON_VIDEO_FORWARD_RENDERER_H_

#include <octoon/hal/graphics_context.h>
#include <octoon/video/render_scene.h>

#include "forward_output.h"
#include "forward_pipeline.h"
#include "forward_scene_controller.h"

namespace octoon::video
{
	class OCTOON_EXPORT ForwardRenderer final
	{
	public:
		ForwardRenderer(const hal::GraphicsContextPtr& context) noexcept;
		virtual ~ForwardRenderer() noexcept;

		void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

		void render(RenderScene* scene) noexcept;

	private:
		void prepareScene(RenderScene* scene) noexcept;

	private:
		ForwardRenderer(const ForwardRenderer&) = delete;
		ForwardRenderer& operator=(const ForwardRenderer&) = delete;

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		ForwardScene profile_;
		hal::GraphicsContextPtr context_;
		std::unique_ptr<ForwardPipeline> pipeline_;
		std::unique_ptr<ForwardSceneController> controller_;
	};
}

#endif