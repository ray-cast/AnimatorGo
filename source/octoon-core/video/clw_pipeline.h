#ifndef OCTOON_VIDEO_CLW_PIPELINE_H_
#define OCTOON_VIDEO_CLW_PIPELINE_H_

#include <CLW.h>
#include "pipeline.h"

namespace octoon::video
{
	class ClwPipeline : public Pipeline
	{
	public:
		ClwPipeline(CLWContext context) noexcept;
		virtual ~ClwPipeline() noexcept;

	private:
		CLWContext context_;
	};
}

#endif