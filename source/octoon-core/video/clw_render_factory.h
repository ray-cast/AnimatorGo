#ifndef OCTOON_CLW_RENDER_FACTORY_H_
#define OCTOON_CLW_RENDER_FACTORY_H_

#include <CLW.h>

#include "render_factory.h"
#include "cl_program_manager.h"

namespace RadeonRays
{
	class IntersectionApi;
}

namespace octoon
{
	class ClwRenderFactory : public RenderFactory
	{
	public:
		ClwRenderFactory(CLWContext context, const std::filesystem::path& cache_path) noexcept;
		virtual ~ClwRenderFactory() noexcept;

		virtual std::unique_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) override;
		virtual std::unique_ptr<Output> createTextureOutput(std::uint32_t texture, std::uint32_t w, std::uint32_t h) override;

		virtual std::unique_ptr<SceneController> createSceneController() override;
		virtual std::unique_ptr<Pipeline> createPipeline() override;

	private:
		ClwRenderFactory(ClwRenderFactory const&) = delete;
		ClwRenderFactory const& operator = (ClwRenderFactory const&) = delete;

	private:
		CLWContext context_;
		CLProgramManager programManager_;
		std::shared_ptr<RadeonRays::IntersectionApi> intersector_;
	};
}

#endif