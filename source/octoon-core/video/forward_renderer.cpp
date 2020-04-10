#include <octoon/video/forward_renderer.h>
#include <octoon/video/forward_scene.h>

namespace octoon::video
{
	ForwardRenderer::ForwardRenderer(const hal::GraphicsContextPtr& context) noexcept
		: context_(context)
		, pipeline_(std::make_unique<ForwardPipeline>(context))
		, controller_(std::make_unique<ForwardSceneController>(context))
		, width_(0)
		, height_(0)
	{
	}

	ForwardRenderer::~ForwardRenderer() noexcept
	{
	}

	void
	ForwardRenderer::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (width_ != w || height_ != h)
		{
			this->width_ = w;
			this->height_ = h;
			this->pipeline_->setupFramebuffers(this->width_, this->height_);
		}
	}
	
	void
	ForwardRenderer::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
	{
		w = this->width_;
		h = this->height_;
	}

	const hal::GraphicsFramebufferPtr&
	ForwardRenderer::getFramebuffer() const noexcept
	{
		return this->pipeline_->getFramebuffer();
	}

	void
	ForwardRenderer::prepareScene(RenderScene* scene) noexcept
	{
		this->controller_->compileScene(scene);
	}

	void
	ForwardRenderer::render(RenderScene* scene) noexcept
	{
		this->prepareScene(scene);
		this->pipeline_->render(this->controller_->getCachedScene(scene));
	}
}