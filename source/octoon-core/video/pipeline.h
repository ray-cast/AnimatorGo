#ifndef OCTOON_VIDEO_PIPELINE_H_
#define OCTOON_VIDEO_PIPELINE_H_

#include <octoon/math/math.h>

namespace octoon::video
{
	class Pipeline
	{
	public:
		Pipeline() noexcept;
		virtual ~Pipeline() noexcept;
	};
}

#endif