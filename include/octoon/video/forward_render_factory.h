#ifndef OCTOON_VIDEO_FORWARD_RENDER_FACTORY_H_
#define OCTOON_VIDEO_FORWARD_RENDER_FACTORY_H_

#include <octoon/hal/graphics_context.h>
#include "render_factory.h"

namespace octoon::video
{
	class ForwardRenderFactory : public RenderFactory
	{
	public:
		ForwardRenderFactory(const hal::GraphicsContextPtr& context, std::string_view cache_path = "") noexcept;
		virtual ~ForwardRenderFactory() noexcept;

		virtual std::unique_ptr<Output> createOutput(std::uint32_t w, std::uint32_t h) const override;
		virtual std::unique_ptr<SceneController> createSceneController() const override;
		virtual std::unique_ptr<Pipeline> createPipeline() const override;

	private:
		ForwardRenderFactory(ForwardRenderFactory const&) = delete;
		ForwardRenderFactory const& operator = (ForwardRenderFactory const&) = delete;

	private:
		hal::GraphicsContextPtr context_;
	};
}

#endif