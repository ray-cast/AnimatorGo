#include <octoon/video/renderer.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/forward_renderer.h>

#include <octoon/runtime/except.h>

#include "rtx_manager.h"

namespace octoon
{
	OctoonImplementSingleton(Renderer)

	Renderer::Renderer() noexcept
		: width_(0)
		, height_(0)
		, enableGlobalIllumination_(false)
		, numBounces_(3)
	{
	}

	Renderer::~Renderer() noexcept
	{
		this->close();
	}

	void
	Renderer::setup(const hal::GraphicsContextPtr& context, std::uint32_t w, std::uint32_t h) except
	{
		width_ = w;
		height_ = h;
		context_ = std::make_shared<ScriptableRenderContext>(context);
		forwardRenderer_ = std::make_unique<ForwardRenderer>();
	}

	void
	Renderer::close() noexcept
	{
		if (this->rtxManager_)
			this->rtxManager_.reset();
		this->forwardRenderer_.reset();
		this->context_.reset();
	}

	void
	Renderer::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		width_ = w;
		height_ = h;
	}

	void
	Renderer::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
	{
		w = width_;
		h = height_;
	}

	void
	Renderer::readColorBuffer(math::float3 data[])
	{
		return this->rtxManager_->readColorBuffer(data);
	}

	void
	Renderer::readAlbedoBuffer(math::float3 data[])
	{
		return this->rtxManager_->readAlbedoBuffer(data);
	}

	void
	Renderer::readNormalBuffer(math::float3 data[])
	{
		return this->rtxManager_->readNormalBuffer(data);
	}

	const hal::GraphicsFramebufferPtr&
	Renderer::getFramebuffer() const noexcept
	{
		assert(this->forwardRenderer_);

		if (this->enableGlobalIllumination_)
			return this->rtxManager_->getFramebuffer();
		else
			return this->forwardRenderer_->getFramebuffer();
	}

	const std::shared_ptr<ScriptableRenderContext>&
	Renderer::getScriptableRenderContext() const noexcept
	{
		return this->context_;
	}

	void
	Renderer::setMaxBounces(std::uint32_t num_bounces)
	{
		numBounces_ = num_bounces;

		if (rtxManager_)
			rtxManager_->setMaxBounces(num_bounces);
	}

	std::uint32_t
	Renderer::getMaxBounces() const
	{
		return numBounces_;
	}

	void
	Renderer::setOverrideMaterial(const std::shared_ptr<Material>& material) noexcept
	{
		this->overrideMaterial_ = material;
	}

	const std::shared_ptr<Material>&
	Renderer::getOverrideMaterial() const noexcept
	{
		return this->overrideMaterial_;
	}

	void
	Renderer::beginFrameRendering(const std::shared_ptr<RenderScene>& scene, const std::vector<Camera*>& camera) noexcept
	{
		scene->sortCameras();
	}

	void 
	Renderer::endFrameRendering(const std::shared_ptr<RenderScene>& scene, const std::vector<Camera*>& camera) noexcept
	{
	}

	void
	Renderer::beginCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept
	{
		scene->setMainCamera(camera);

		camera->onRenderBefore(*camera);

		for (auto& it : scene->getGeometries())
		{
			if (!it->getVisible())
				continue;

			if (camera->getLayer() == it->getLayer())
				it->onRenderBefore(*camera);
		}
	}

	void
	Renderer::endCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept
	{
		for (auto& it : scene->getLights())
		{
			if (!it->getVisible())
				continue;

			if (camera->getLayer() == it->getLayer())
				it->setDirty(false);
		}

		for (auto& it : scene->getGeometries())
		{
			if (!it->getVisible())
				continue;

			if (camera->getLayer() == it->getLayer())
			{
				it->onRenderAfter(*camera);

				auto mesh = it->getMesh();
				if (mesh)
					mesh->setDirty(false);

				for (auto& material : it->getMaterials())
				{
					if (material)
						material->setDirty(false);
				}

				it->setDirty(false);
			}
		}

		camera->onRenderAfter(*camera);
		camera->setDirty(false);
	}

	void
	Renderer::renderSingleCamera(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept
	{		
		scene->sortGeometries();

		if (scene->getGlobalIllumination())
		{
			if (!rtxManager_)
			{
				rtxManager_ = std::make_unique<RtxManager>();
				rtxManager_->setMaxBounces(this->getMaxBounces());
			}

			this->rtxManager_->render(this->context_, scene);
		}
		else
		{
			context_->compileScene(scene);

			forwardRenderer_->render(this->context_, this->context_->getRenderingData());
		}
	}

	void
	Renderer::render(const std::shared_ptr<RenderScene>& scene) noexcept(false)
	{
		this->beginFrameRendering(scene, scene->getCameras());

		for (auto& camera : scene->getCameras())
		{
			this->beginCameraRendering(scene, camera);

			this->renderSingleCamera(scene, camera);

			this->endCameraRendering(scene, camera);
		}

		this->endFrameRendering(scene, scene->getCameras());
	}
}