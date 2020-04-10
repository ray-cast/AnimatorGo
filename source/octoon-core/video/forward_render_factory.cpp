#include <octoon/video/forward_render_factory.h>
#include <octoon/video/forward_output.h>
#include <octoon/video/forward_pipeline.h>
#include <octoon/video/forward_scene_controller.h>

namespace octoon::video
{
	ForwardRenderFactory::ForwardRenderFactory(const hal::GraphicsContextPtr& context, std::string_view cache_path) noexcept
		: context_(context)
	{
	}

	ForwardRenderFactory::~ForwardRenderFactory() noexcept
	{
	}

	std::unique_ptr<Output>
	ForwardRenderFactory::createOutput(std::uint32_t w, std::uint32_t h) const
	{
		return std::make_unique<ForwardOutput>(context_, w, h);
	}

	std::unique_ptr<SceneController>
	ForwardRenderFactory::createSceneController() const
	{
		return std::make_unique<ForwardSceneController>(context_);
	}

	std::unique_ptr<Pipeline>
	ForwardRenderFactory::createPipeline() const
	{
		return std::make_unique<ForwardPipeline>(context_);
	}
}