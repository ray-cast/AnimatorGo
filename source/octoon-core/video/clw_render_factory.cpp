#include "clw_render_factory.h"
#include "clw_output.h"
#include "clw_texture_output.h"
#include "clw_scene_controller.h"

#include "monte_carlo_renderer.h"
#include "path_tracing_estimator.h"

#include <radeon_rays.h>
#include <radeon_rays_cl.h>

namespace octoon
{
	ClwRenderFactory::ClwRenderFactory(CLWContext context, std::string_view cache_path) noexcept
		: context_(context)
		, programManager_(cache_path)
		, intersector_(RadeonRays::CreateFromOpenClContext(context, context.GetDevice(0).GetID(), context.GetCommandQueue(0)), RadeonRays::IntersectionApi::Delete)
	{
	}

	ClwRenderFactory::~ClwRenderFactory() noexcept
	{
	}

	std::unique_ptr<Output>
	ClwRenderFactory::createOutput(std::uint32_t w, std::uint32_t h) const
	{
		return std::make_unique<ClwOutput>(context_, w, h);
	}

	std::unique_ptr<Output>
	ClwRenderFactory::createTextureOutput(std::uint32_t texture, std::uint32_t w, std::uint32_t h) const
	{
		return std::make_unique<ClwTextureOutput>(context_, texture, w, h);
	}

	std::unique_ptr<SceneController>
	ClwRenderFactory::createSceneController() const
	{
		return std::make_unique<ClwSceneController>(context_, intersector_, &programManager_);
	}

	std::unique_ptr<Pipeline>
	ClwRenderFactory::createPipeline() const
	{
		return std::make_unique<MonteCarloRenderer>(context_, &programManager_, std::make_unique<PathTracingEstimator>(context_, intersector_, &programManager_));
	}
}