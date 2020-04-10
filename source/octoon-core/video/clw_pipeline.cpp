#include "clw_pipeline.h"

namespace octoon::video
{
	ClwPipeline::ClwPipeline(CLWContext context) noexcept
		: context_(context)
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