#ifndef OCTOON_RENDER_FACTORY_H_
#define OCTOON_RENDER_FACTORY_H_

#include <memory>

#include <octoon/math/math.h>
#include <octoon/video/output.h>
#include <octoon/video/scene_controller.h>
#include <octoon/video/pipeline.h>

namespace octoon
{
	class RenderFactory
	{
	public:
		RenderFactory() noexcept = default;
		virtual ~RenderFactory() noexcept = default;

		virtual std::unique_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) = 0;
		virtual std::unique_ptr<Output> createTextureOutput(std::uint32_t texture, std::uint32_t w, std::uint32_t h) = 0;
		virtual std::unique_ptr<SceneController> createSceneController() = 0;
		virtual std::unique_ptr<Pipeline> createPipeline() = 0;

	private:
		RenderFactory(RenderFactory const&) = delete;
		RenderFactory const& operator = (RenderFactory const&) = delete;
	};
}

#endif