#include <octoon/video/renderer.h>
#include <octoon/video/render_scene.h>

#include <octoon/video/forward_material.h>
#include <octoon/video/forward_renderer.h>

#include <octoon/runtime/except.h>

#include "rtx_manager.h"

namespace octoon::video
{
	OctoonImplementSingleton(Renderer)

	Renderer::Renderer() noexcept
		: width_(0)
		, height_(0)
		, sortObjects_(true)
		, enableGlobalIllumination_(false)
	{
	}

	Renderer::~Renderer() noexcept
	{
		this->close();
	}

	void
	Renderer::setup(const hal::GraphicsContextPtr& context, std::uint32_t w, std::uint32_t h) except
	{
		context_ = context;
		forwardRenderer_ = std::make_unique<ForwardRenderer>(context);

		this->setFramebufferSize(w, h);
	}

	void
	Renderer::close() noexcept
	{
		this->rtxManager_.reset();
		this->forwardRenderer_.reset();
		this->context_.reset();
	}

	void
	Renderer::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (width_ != w || height_ != h)
		{
			width_ = w;
			height_ = h;
			this->forwardRenderer_->setFramebufferSize(w, h);
		}
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

	void
	Renderer::setSortObjects(bool sortObject) noexcept
	{
		this->sortObjects_ = sortObject;
	}

	bool
	Renderer::getSortObject() const noexcept
	{
		return this->sortObjects_;
	}

	void
	Renderer::setOverrideMaterial(const std::shared_ptr<material::Material>& material) noexcept
	{
		this->overrideMaterial_ = material;
	}

	const std::shared_ptr<material::Material>&
	Renderer::getOverrideMaterial() const noexcept
	{
		return this->overrideMaterial_;
	}

	hal::GraphicsInputLayoutPtr
	Renderer::createInputLayout(const hal::GraphicsInputLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createInputLayout(desc);
	}

	hal::GraphicsDataPtr
	Renderer::createGraphicsData(const hal::GraphicsDataDesc& desc) noexcept
	{
		return this->context_->getDevice()->createGraphicsData(desc);
	}

	hal::GraphicsTexturePtr
	Renderer::createTexture(const hal::GraphicsTextureDesc& desc) noexcept
	{
		return this->context_->getDevice()->createTexture(desc);
	}

	hal::GraphicsSamplerPtr
	Renderer::createSampler(const hal::GraphicsSamplerDesc& desc) noexcept
	{
		return this->context_->getDevice()->createSampler(desc);
	}

	hal::GraphicsFramebufferPtr
	Renderer::createFramebuffer(const hal::GraphicsFramebufferDesc& desc) noexcept
	{
		return this->context_->getDevice()->createFramebuffer(desc);
	}

	hal::GraphicsFramebufferLayoutPtr
	Renderer::createFramebufferLayout(const hal::GraphicsFramebufferLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createFramebufferLayout(desc);
	}

	hal::GraphicsShaderPtr
	Renderer::createShader(const hal::GraphicsShaderDesc& desc) noexcept
	{
		return this->context_->getDevice()->createShader(desc);
	}

	hal::GraphicsProgramPtr
	Renderer::createProgram(const hal::GraphicsProgramDesc& desc) noexcept
	{
		return this->context_->getDevice()->createProgram(desc);
	}

	hal::GraphicsStatePtr
	Renderer::createRenderState(const hal::GraphicsStateDesc& desc) noexcept
	{
		return this->context_->getDevice()->createRenderState(desc);
	}

	hal::GraphicsPipelinePtr
	Renderer::createRenderPipeline(const hal::GraphicsPipelineDesc& desc) noexcept
	{
		return this->context_->getDevice()->createRenderPipeline(desc);
	}

	hal::GraphicsDescriptorSetPtr
	Renderer::createDescriptorSet(const hal::GraphicsDescriptorSetDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorSet(desc);
	}

	hal::GraphicsDescriptorSetLayoutPtr
	Renderer::createDescriptorSetLayout(const hal::GraphicsDescriptorSetLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorSetLayout(desc);
	}

	hal::GraphicsDescriptorPoolPtr
	Renderer::createDescriptorPool(const hal::GraphicsDescriptorPoolDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorPool(desc);
	}

	void
	Renderer::generateMipmap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		this->context_->generateMipmap(texture);
	}

	void
	Renderer::render(const std::shared_ptr<RenderScene>& scene) noexcept(false)
	{
		if (this->sortObjects_)
		{
			scene->sortCameras();
			scene->sortGeometries();
		}

		for (auto& camera : scene->getCameras())
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

			if (this->sortObjects_)
				scene->sortGeometries();

			if (scene->getGlobalIllumination())
			{
				if (!rtxManager_)
				{
					rtxManager_ = std::make_unique<RtxManager>();
					rtxManager_->setGraphicsContext(this->context_);
				}

				this->rtxManager_->render(scene);
			}
			else
			{
				forwardRenderer_->render(scene);
			}

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
	}
}