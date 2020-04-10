#ifndef OCTOON_VIDEO_CLW_RENDER_FACTORY_H_
#define OCTOON_VIDEO_CLW_RENDER_FACTORY_H_

#include <CLW.h>

#include "render_factory.h"

namespace RadeonRays
{
	class IntersectionApi;
}

namespace octoon::video
{
	class ClwRenderFactory : public RenderFactory
	{
	public:
		ClwRenderFactory(CLWContext context, std::string_view cache_path = "") noexcept;
		virtual ~ClwRenderFactory() noexcept;

		virtual std::unique_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) const override;
		virtual std::unique_ptr<SceneController> createSceneController() const override;
		virtual std::unique_ptr<Pipeline> createPipeline() const override;

	private:
		ClwRenderFactory(ClwRenderFactory const&) = delete;
		ClwRenderFactory const& operator = (ClwRenderFactory const&) = delete;

	private:
		CLWContext context_;

		std::shared_ptr<RadeonRays::IntersectionApi> intersector_;
	};
}

#endif