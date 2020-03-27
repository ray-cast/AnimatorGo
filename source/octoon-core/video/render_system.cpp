#include <octoon/video/render_system.h>
#include <octoon/video/render_pipeline.h>
#include <octoon/video/render_scene.h>

#include <octoon/runtime/except.h>

using namespace octoon::hal;

namespace octoon::video
{
	OctoonImplementSingleton(RenderSystem)

	RenderSystem::RenderSystem() noexcept
		: width_(0)
		, height_(0)
		, fbo_(0)
		, colorTexture_(0)
		, depthTexture_(0)
		, sortObjects_(true)
	{
	}

	RenderSystem::~RenderSystem() noexcept
	{
		this->close();
	}

	void
	RenderSystem::setup(const GraphicsDevicePtr& device, std::uint32_t w, std::uint32_t h) except
	{
		device_ = device;

		this->setFramebufferSize(w, h);
	}

	void
	RenderSystem::close() noexcept
	{
		fbo_.reset();
		colorTexture_.reset();
		depthTexture_.reset();
		device_.reset();
	}

	void
	RenderSystem::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (width_ != w || height_ != h)
		{
			this->setupFramebuffers(w, h);

			width_ = w;
			height_ = h;
		}
	}

	void
	RenderSystem::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
	{
		w = width_;
		h = height_;
	}

	const GraphicsFramebufferPtr&
	RenderSystem::getFramebuffer() const noexcept
	{
		return fbo_;
	}

	void
	RenderSystem::setSortObjects(bool sortObject) noexcept
	{
		this->sortObjects_ = sortObject;
	}

	bool
	RenderSystem::getSortObject() const noexcept
	{
		return this->sortObjects_;
	}

	void
	RenderSystem::setOverrideMaterial(const std::shared_ptr<material::Material>& material) noexcept
	{
		this->overrideMaterial_ = material;
	}

	std::shared_ptr<material::Material>
	RenderSystem::getOverrideMaterial() const noexcept
	{
		return this->overrideMaterial_;
	}

	GraphicsInputLayoutPtr
	RenderSystem::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
	{
		assert(device_);
		return device_->createInputLayout(desc);
	}

	GraphicsDataPtr
	RenderSystem::createGraphicsData(const GraphicsDataDesc& desc) noexcept
	{
		assert(device_);
		return device_->createGraphicsData(desc);
	}

	GraphicsTexturePtr
	RenderSystem::createTexture(const GraphicsTextureDesc& desc) noexcept
	{
		assert(device_);
		return device_->createTexture(desc);
	}

	GraphicsSamplerPtr
	RenderSystem::createSampler(const GraphicsSamplerDesc& desc) noexcept
	{
		assert(device_);
		return device_->createSampler(desc);
	}

	GraphicsFramebufferPtr
	RenderSystem::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
	{
		assert(device_);
		return device_->createFramebuffer(desc);
	}

	GraphicsFramebufferLayoutPtr
	RenderSystem::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
	{
		assert(device_);
		return device_->createFramebufferLayout(desc);
	}

	GraphicsShaderPtr
	RenderSystem::createShader(const GraphicsShaderDesc& desc) noexcept
	{
		assert(device_);
		return device_->createShader(desc);
	}

	GraphicsProgramPtr
	RenderSystem::createProgram(const GraphicsProgramDesc& desc) noexcept
	{
		assert(device_);
		return device_->createProgram(desc);
	}

	GraphicsStatePtr
	RenderSystem::createRenderState(const GraphicsStateDesc& desc) noexcept
	{
		assert(device_);
		return device_->createRenderState(desc);
	}

	GraphicsPipelinePtr
	RenderSystem::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
	{
		assert(device_);
		return device_->createRenderPipeline(desc);
	}

	GraphicsDescriptorSetPtr
	RenderSystem::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
	{
		assert(device_);
		return device_->createDescriptorSet(desc);
	}

	GraphicsDescriptorSetLayoutPtr
	RenderSystem::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
	{
		assert(device_);
		return device_->createDescriptorSetLayout(desc);
	}

	GraphicsDescriptorPoolPtr
	RenderSystem::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
	{
		assert(device_);
		return device_->createDescriptorPool(desc);
	}

	void
	RenderSystem::renderObjects(hal::GraphicsContext& context, const std::vector<RenderObject*>& objects, const camera::Camera& camera) noexcept
	{
		for (auto& object : objects)
		{
			if (camera.getLayer() != object->getLayer())
				continue;

			object->onRenderBefore(camera);

			auto geometry = object->downcast<geometry::Geometry>();
			if (!geometry->getVisible())
				continue;

			if (!this->setProgram(context, this->overrideMaterial_ ? this->overrideMaterial_ : geometry->getMaterial(), camera, *geometry))
				continue;

			if (geometry->getVertexBuffer())
			{
				context.setVertexBufferData(0, geometry->getVertexBuffer(), 0);
				context.setIndexBufferData(geometry->getIndexBuffer(), 0, hal::GraphicsIndexType::UInt32);

				auto indices = geometry->getNumIndices();
				if (indices > 0)
					context.drawIndexed(indices, 1, 0, 0, 0);
				else
					context.draw(geometry->getNumVertices(), 1, 0, 0);
			}

			object->onRenderAfter(camera);
		}
	}

	bool
	RenderSystem::setProgram(hal::GraphicsContext& context, const std::shared_ptr<material::Material>& material, const camera::Camera& camera, const geometry::Geometry& geometry)
	{
		if (material)
		{
			auto& pipeline = pipelines_[((std::intptr_t)material.get())];
			if (!pipeline)
				pipeline = std::make_shared<RenderPipeline>(material);

			pipeline->update(camera, geometry);

			context.setRenderPipeline(pipeline->getPipeline());
			context.setDescriptorSet(pipeline->getDescriptorSet());

			return true;
		}
		else
		{
			return false;
		}
	}

	void
	RenderSystem::render(hal::GraphicsContext& context) noexcept
	{
		if (this->sortObjects_)
		{
			RenderScene::instance()->sortCameras();
			RenderScene::instance()->sortRenderObjects();
		}

		for (auto& camera : RenderScene::instance()->getCameraList())
		{
			camera->onRenderBefore(*camera);

#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			auto framebuffer = camera->getFramebuffer();
			if (framebuffer)
				context.setFramebuffer(framebuffer);
			else
				context.setFramebuffer(fbo_);
#endif

			context.setViewport(0, camera->getPixelViewport());
			context.clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);

			this->renderObjects(context, RenderScene::instance()->getRenderObjects(), *camera);

			camera->onRenderAfter(*camera);

#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			if (camera->getBlitToScreen())
			{
				auto& v = camera->getPixelViewport();
				if (framebuffer)
					context.blitFramebuffer(framebuffer, v, nullptr, v);
				else
					context.blitFramebuffer(fbo_, v, nullptr, v);
			}

			if (framebuffer)
			{
				auto& swapFramebuffer = camera->getSwapFramebuffer();
				if (swapFramebuffer)
				{
					math::float4 v1(0, 0, (float)framebuffer->getGraphicsFramebufferDesc().getWidth(), (float)framebuffer->getGraphicsFramebufferDesc().getHeight());
					math::float4 v2(0, 0, (float)swapFramebuffer->getGraphicsFramebufferDesc().getWidth(), (float)swapFramebuffer->getGraphicsFramebufferDesc().getHeight());
					context.blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
				}
			}
#endif
		}
	}

	void
	RenderSystem::setupFramebuffers(std::uint32_t w, std::uint32_t h) except
	{
		GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
		framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R32G32B32SFloat));
		framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::X8_D24UNormPack32));

		try
		{
			GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexMultisample(4);
			colorTextureDesc.setTexDim(GraphicsTextureDim::Texture2DMultisample);
			colorTextureDesc.setTexFormat(GraphicsFormat::R32G32B32SFloat);
			colorTexture_ = device_->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexMultisample(4);
			depthTextureDesc.setTexDim(GraphicsTextureDim::Texture2DMultisample);
			depthTextureDesc.setTexFormat(GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = device_->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = device_->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
		catch (...)
		{
			GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
			colorTexture_ = device_->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexFormat(GraphicsFormat::D16UNorm);
			depthTexture_ = device_->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = device_->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
	}
}