#include "clw_pipeline.h"

namespace octoon::video
{
	ClwPipeline::ClwPipeline(CLWContext context, std::shared_ptr<RadeonRays::IntersectionApi> api) noexcept
		: context_(context)
		, api_(api)
	{
	}

	ClwPipeline::~ClwPipeline() noexcept
	{
	}

	void
	ClwPipeline::render(CompiledScene& scene) noexcept
	{
	}
}