#include <octoon/video/forward_renderer.h>
#include <octoon/video/forward_scene.h>

namespace octoon::video
{
	ForwardRenderer::ForwardRenderer() noexcept
		: pipeline_(std::make_unique<ForwardPipeline>())
		, controller_(std::make_unique<ForwardSceneController>())
	{
	}

	ForwardRenderer::~ForwardRenderer() noexcept
	{
		this->profile_.reset();
	}

	const hal::GraphicsFramebufferPtr&
	ForwardRenderer::getFramebuffer() const noexcept
	{
		return this->pipeline_->getFramebuffer();
	}

	void
	ForwardRenderer::render(const std::shared_ptr<ScriptableRenderContext>& context, const std::shared_ptr<RenderScene>& scene) noexcept
	{
		this->controller_->cleanCache();
		this->controller_->compileScene(context, scene);
		this->pipeline_->render(context, this->controller_->getCachedScene(scene));
	}
}