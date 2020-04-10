#ifndef OCTOON_VIDEO_PIPELINE_H_
#define OCTOON_VIDEO_PIPELINE_H_

#include <octoon/video/compiled_scene.h>

namespace octoon::video
{
	class Pipeline
	{
	public:
		Pipeline() noexcept;
		virtual ~Pipeline() noexcept;

		virtual void render(CompiledScene& scene) noexcept = 0;
	};
}

#endif