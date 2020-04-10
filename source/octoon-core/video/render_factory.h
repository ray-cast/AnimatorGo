#ifndef OCTOON_VIDEO_RENDER_FACTORY_H_
#define OCTOON_VIDEO_RENDER_FACTORY_H_

#include <octoon/math/math.h>
#include <memory>

#include "output.h"
#include "pipeline.h"
#include "scene_controller.h"

namespace octoon::video
{
	class RenderFactory
	{
	public:
		RenderFactory() noexcept;
		virtual ~RenderFactory() noexcept;

		virtual std::unique_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) const = 0;
		virtual std::unique_ptr<SceneController> createSceneController() const = 0;
		virtual std::unique_ptr<Pipeline> createPipeline() const = 0;

	private:
		RenderFactory(RenderFactory const&) = delete;
		RenderFactory const& operator = (RenderFactory const&) = delete;
	};
}

#endif