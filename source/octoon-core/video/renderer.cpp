#include <octoon/video/renderer.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/forward_renderer.h>

#include <octoon/runtime/except.h>

#include "config_manager.h"

#include "spdlog/spdlog.h"

namespace octoon
{
	OctoonImplementSingleton(Renderer)

	Renderer::Renderer() noexcept
		: width_(0)
		, height_(0)
		, enableGlobalIllumination_(false)
		, numBounces_(5)
	{
	}

	Renderer::~Renderer() noexcept
	{
		this->close();
	}

	void
	Renderer::open(const GraphicsContextPtr& context, std::uint32_t w, std::uint32_t h) except
	{
		width_ = w;
		height_ = h;
		context_ = context;
		forwardRenderer_ = std::make_unique<ForwardRenderer>(context);
		forwardRenderer_->setFramebufferSize(w, h);
	}

	void
	Renderer::close() noexcept
	{
		if (this->pathRenderer_)
			this->pathRenderer_.reset();
		this->forwardRenderer_.reset();
		this->context_.reset();
	}

	void
	Renderer::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		width_ = w;
		height_ = h;

		if (this->forwardRenderer_)
			this->forwardRenderer_->setFramebufferSize(w, h);

		if (this->pathRenderer_)
			this->pathRenderer_->setFramebufferSize(w, h);
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
		return this->pathRenderer_->readColorBuffer(data);
	}

	void
	Renderer::readAlbedoBuffer(math::float3 data[])
	{
		return this->pathRenderer_->readAlbedoBuffer(data);
	}

	void
	Renderer::readNormalBuffer(math::float3 data[])
	{
		return this->pathRenderer_->readNormalBuffer(data);
	}

	const GraphicsFramebufferPtr&
	Renderer::getFramebuffer() const noexcept
	{
		assert(this->forwardRenderer_);

		if (this->enableGlobalIllumination_)
			return this->pathRenderer_->getFramebuffer();
		else
			return this->forwardRenderer_->getFramebuffer();
	}

	const GraphicsContextPtr&
	Renderer::getScriptableRenderContext() const noexcept
	{
		return this->context_;
	}

	const GraphicsDevicePtr
	Renderer::getGraphicsDevice() const noexcept
	{
		return this->context_->getDevice();
	}

	void
	Renderer::setMaxBounces(std::uint32_t num_bounces)
	{
		numBounces_ = num_bounces;

		if (pathRenderer_)
			pathRenderer_->setMaxBounces(num_bounces);
	}

	std::uint32_t
	Renderer::getMaxBounces() const
	{
		return numBounces_;
	}

	void
	Renderer::setCachePath(const std::filesystem::path& path)
	{
		if (pathRenderer_)
			return pathRenderer_->setCachePath(path);
		cachePath_ = path;
	}

	const std::filesystem::path&
	Renderer::getCachePath() const
	{
		return cachePath_;
	}

	std::uint32_t
	Renderer::getSampleCounter() const
	{
		if (pathRenderer_)
			return pathRenderer_->getSampleCounter();
		return 0;
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
	Renderer::beginFrameRendering(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		scene->sortCameras();
	}

	void 
	Renderer::endFrameRendering(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		scene->setSceneDirty(false);
	}

	void
	Renderer::beginCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false)
	{
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
	Renderer::endCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false)
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
	Renderer::renderSingleCamera(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false)
	{
		scene->setMainCamera(camera);
		scene->sortGeometries();

		if (scene->getGlobalIllumination())
		{
			if (!pathRenderer_)
			{
				pathRenderer_ = std::make_unique<ConfigManager>(this->context_->getDevice()->getSystemInfo().graphicsDeviceName, cachePath_);
				pathRenderer_->setMaxBounces(this->getMaxBounces());
				pathRenderer_->setFramebufferSize(this->width_, this->height_);

				spdlog::info("Forward render using: " + pathRenderer_->getCurrentRenderDeviceName());
			}

			this->pathRenderer_->render(this->context_, scene);
		}
		else
		{
			this->forwardRenderer_->render(scene);
		}
	}

	void
	Renderer::render(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false)
	{
		this->beginFrameRendering(scene);

		this->beginCameraRendering(scene, camera);
		this->renderSingleCamera(scene, camera);
		this->endCameraRendering(scene, camera);

		this->endFrameRendering(scene);
	}

	void
	Renderer::render(const std::shared_ptr<RenderScene>& scene) noexcept(false)
	{
		this->beginFrameRendering(scene);

		for (auto& camera : scene->getCameras())
		{
			this->beginCameraRendering(scene, camera);

			this->renderSingleCamera(scene, camera);

			this->endCameraRendering(scene, camera);
		}

		this->endFrameRendering(scene);
	}
}