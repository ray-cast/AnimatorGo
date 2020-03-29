#include <octoon/video/renderer.h>
#include <octoon/video/render_pipeline.h>
#include <octoon/video/render_scene.h>

#include <octoon/light/ambient_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/disk_light.h>
#include <octoon/light/rectangle_light.h>
#include <octoon/light/environment_light.h>
#include <octoon/light/tube_light.h>

#include <octoon/runtime/except.h>

using namespace octoon::hal;

namespace octoon::video
{
	OctoonImplementSingleton(Renderer)

	Renderer::Renderer() noexcept
		: width_(0)
		, height_(0)
		, fbo_(0)
		, colorTexture_(0)
		, depthTexture_(0)
		, sortObjects_(true)
	{
	}

	Renderer::~Renderer() noexcept
	{
		this->close();
	}

	void
	Renderer::setup(const GraphicsDevicePtr& device, std::uint32_t w, std::uint32_t h) except
	{
		device_ = device;

		hal::GraphicsDataDesc directionalLightDesc;
		directionalLightDesc.setType(hal::GraphicsDataType::UniformBuffer);
		directionalLightDesc.setStream(nullptr);
		directionalLightDesc.setStreamSize(sizeof(light::DirectionalLight));
		directionalLightDesc.setUsage(hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit);

		this->context_.light.directionLightBuffer = this->createGraphicsData(directionalLightDesc);

		this->setFramebufferSize(w, h);
	}

	void
	Renderer::close() noexcept
	{
		this->context_.light.directionLightBuffer.reset();
		this->buffers_.clear();
		this->pipelines_.clear();
		fbo_.reset();
		currentBuffer_.reset();
		colorTexture_.reset();
		depthTexture_.reset();
		device_.reset();
	}

	void
	Renderer::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (width_ != w || height_ != h)
		{
			this->setupFramebuffers(w, h);

			width_ = w;
			height_ = h;
		}
	}

	void
	Renderer::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
	{
		w = width_;
		h = height_;
	}

	const GraphicsFramebufferPtr&
	Renderer::getFramebuffer() const noexcept
	{
		return fbo_;
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

	std::shared_ptr<material::Material>
	Renderer::getOverrideMaterial() const noexcept
	{
		return this->overrideMaterial_;
	}

	GraphicsInputLayoutPtr
	Renderer::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
	{
		assert(device_);
		return device_->createInputLayout(desc);
	}

	GraphicsDataPtr
	Renderer::createGraphicsData(const GraphicsDataDesc& desc) noexcept
	{
		assert(device_);
		return device_->createGraphicsData(desc);
	}

	GraphicsTexturePtr
	Renderer::createTexture(const GraphicsTextureDesc& desc) noexcept
	{
		assert(device_);
		return device_->createTexture(desc);
	}

	GraphicsSamplerPtr
	Renderer::createSampler(const GraphicsSamplerDesc& desc) noexcept
	{
		assert(device_);
		return device_->createSampler(desc);
	}

	GraphicsFramebufferPtr
	Renderer::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
	{
		assert(device_);
		return device_->createFramebuffer(desc);
	}

	GraphicsFramebufferLayoutPtr
	Renderer::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
	{
		assert(device_);
		return device_->createFramebufferLayout(desc);
	}

	GraphicsShaderPtr
	Renderer::createShader(const GraphicsShaderDesc& desc) noexcept
	{
		assert(device_);
		return device_->createShader(desc);
	}

	GraphicsProgramPtr
	Renderer::createProgram(const GraphicsProgramDesc& desc) noexcept
	{
		assert(device_);
		return device_->createProgram(desc);
	}

	GraphicsStatePtr
	Renderer::createRenderState(const GraphicsStateDesc& desc) noexcept
	{
		assert(device_);
		return device_->createRenderState(desc);
	}

	GraphicsPipelinePtr
	Renderer::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
	{
		assert(device_);
		return device_->createRenderPipeline(desc);
	}

	GraphicsDescriptorSetPtr
	Renderer::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
	{
		assert(device_);
		return device_->createDescriptorSet(desc);
	}

	GraphicsDescriptorSetLayoutPtr
	Renderer::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
	{
		assert(device_);
		return device_->createDescriptorSetLayout(desc);
	}

	GraphicsDescriptorPoolPtr
	Renderer::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
	{
		assert(device_);
		return device_->createDescriptorPool(desc);
	}

	void
	Renderer::prepareLights(hal::GraphicsContext& context, const std::vector<light::Light*>& lights, const camera::Camera& camera) noexcept
	{
		this->lights_.clear();

		this->context_.light.numAmbient = 0;
		this->context_.light.numDirectional = 0;
		this->context_.light.numSpot = 0;
		this->context_.light.numPoint = 0;
		this->context_.light.numEnvironment = 0;
		this->context_.light.numArea = 0;

		this->context_.light.directionalLights.clear();

		for (auto& light : lights)
		{
			if (camera.getLayer() != light->getLayer())
				continue;

			if (light->getVisible())
			{
				light->onRenderBefore(camera);

				if (light->isA<light::AmbientLight>()) {
					this->context_.light.numAmbient++;
				} else if (light->isA<light::DirectionalLight>()) {
					auto it = light->downcast<light::DirectionalLight>();
					DirectionalLight directionLight;
					directionLight.color.set(light->getLightColor() * light->getLightIntensity());
					directionLight.direction.set(light->getForward());
					directionLight.shadow = false;
					directionLight.shadowBias = it->getShadowBias();
					directionLight.shadowRadius = it->getShadowRadius();
					directionLight.shadowMapSize = math::float2::Zero;
					this->context_.light.directionalLights.emplace_back(directionLight);
					this->context_.light.numDirectional++;
				} else if (light->isA<light::SpotLight>()) {
					this->context_.light.numSpot++;
				} else if (light->isA<light::PointLight>()) {
					this->context_.light.numPoint++;
				} else if (light->isA<light::EnvironmentLight>()) {
					this->context_.light.numEnvironment++;
				} else if (light->isA<light::RectangleLight>()) {
					this->context_.light.numArea++;
				}

				light->onRenderAfter(camera);

				this->lights_.emplace_back(light);
			}
		}

		if (this->context_.light.numDirectional)
		{
			auto desc = this->context_.light.directionLightBuffer->getDataDesc();
			if (desc.getStreamSize() < this->context_.light.directionalLights.size() * sizeof(light::DirectionalLight))
			{
				hal::GraphicsDataDesc indiceDesc;
				indiceDesc.setType(hal::GraphicsDataType::UniformBuffer);
				indiceDesc.setStream((std::uint8_t*)this->context_.light.directionalLights.data());
				indiceDesc.setStreamSize(this->context_.light.directionalLights.size() * sizeof(light::DirectionalLight));
				indiceDesc.setUsage(hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit);

				this->context_.light.directionLightBuffer = this->createGraphicsData(indiceDesc);
			}
			else
			{
				void* data;
				if (this->context_.light.directionLightBuffer->map(0, desc.getStreamSize(), &data))
					std::memcpy(data, this->context_.light.directionalLights.data(), this->context_.light.directionalLights.size() * sizeof(light::DirectionalLight));
				this->context_.light.directionLightBuffer->unmap();
			}
		}
	}

	void
	Renderer::renderObjects(hal::GraphicsContext& context, const std::vector<geometry::Geometry*>& geometries, const camera::Camera& camera) noexcept
	{
		for (auto& geometry : geometries)
		{
			if (camera.getLayer() != geometry->getLayer())
				continue;

			if (geometry->getVisible())
			{
				geometry->onRenderBefore(camera);

				if (!this->setProgram(context, this->overrideMaterial_ ? this->overrideMaterial_ : geometry->getMaterial(), camera, *geometry))
					continue;

				if (!this->setBuffer(context, geometry->getMesh(), geometry->getMeshSubset()))
					continue;

				auto indices = currentBuffer_->getNumIndices(geometry->getMeshSubset());
				if (indices > 0)
					context.drawIndexed((std::uint32_t)indices, 1, 0, 0, 0);
				else
					context.draw((std::uint32_t)currentBuffer_->getNumVertices(), 1, 0, 0);

				geometry->onRenderAfter(camera);
			}
		}
	}

	void
	Renderer::render(RenderScene& scene, hal::GraphicsContext& context) noexcept
	{
		if (this->sortObjects_)
		{
			scene.sortCameras();
			scene.sortGeometries();
		}

		for (auto& camera : scene.getCameraList())
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

			this->prepareLights(context, scene.getLights(), *camera);
			this->renderObjects(context, scene.getGeometries(), *camera);

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
	Renderer::setupFramebuffers(std::uint32_t w, std::uint32_t h) except
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

	bool
	Renderer::setBuffer(hal::GraphicsContext& context, const std::shared_ptr<mesh::Mesh>& mesh, std::size_t subset)
	{
		if (mesh)
		{
			auto& buffer = buffers_[((std::intptr_t)mesh.get())];
			if (!buffer)
				buffer = std::make_shared<RenderBuffer>(mesh);

			context.setVertexBufferData(0, buffer->getVertexBuffer(), 0);
			context.setIndexBufferData(buffer->getIndexBuffer(subset), 0, hal::GraphicsIndexType::UInt32);

			this->currentBuffer_ = buffer;

			return true;
		}
		else
		{
			return false;
		}
	}

	bool
	Renderer::setProgram(hal::GraphicsContext& context, const std::shared_ptr<material::Material>& material, const camera::Camera& camera, const geometry::Geometry& geometry)
	{
		if (material)
		{
			auto& pipeline = pipelines_[((std::intptr_t)material.get())];
			if (!pipeline)
				pipeline = std::make_shared<RenderPipeline>(material, this->context_);

			pipeline->update(camera, geometry, this->context_);

			context.setRenderPipeline(pipeline->getPipeline());
			context.setDescriptorSet(pipeline->getDescriptorSet());

			return true;
		}
		else
		{
			return false;
		}
	}
}