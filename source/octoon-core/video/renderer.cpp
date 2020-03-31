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

		this->setFramebufferSize(w, h);
	}

	void
	Renderer::close() noexcept
	{
		this->profile_.light.pointLightBuffer.reset();
		this->profile_.light.spotLightBuffer.reset();
		this->profile_.light.rectangleLightBuffer.reset();
		this->profile_.light.directionLightBuffer.reset();
		this->profile_.light.pointLights.clear();
		this->profile_.light.spotLights.clear();
		this->profile_.light.directionalLights.clear();
		this->profile_.light.rectangleLights.clear();
		this->profile_.light.environmentLights.clear();
		this->buffers_.clear();
		this->pipelines_.clear();
		fbo_.reset();
		currentBuffer_.reset();
		colorTexture_.reset();
		depthTexture_.reset();
		context_.reset();
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
	Renderer::setGraphicsContext(const hal::GraphicsContextPtr& context) noexcept(false)
	{
		this->context_ = context;
	}
	
	const hal::GraphicsContextPtr&
	Renderer::getGraphicsContext() const noexcept(false)
	{
		return this->context_;
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
	Renderer::prepareLights(const std::vector<light::Light*>& lights, const camera::Camera& camera) noexcept
	{
		this->lights_.clear();

		this->profile_.light.numDirectional = 0;
		this->profile_.light.numSpot = 0;
		this->profile_.light.numPoint = 0;
		this->profile_.light.numHemi = 0;
		this->profile_.light.numEnvironment = 0;
		this->profile_.light.numRectangle = 0;

		this->profile_.light.ambientLightColors = math::float3::Zero;
		this->profile_.light.pointLights.clear();
		this->profile_.light.spotLights.clear();
		this->profile_.light.rectangleLights.clear();
		this->profile_.light.directionalLights.clear();
		this->profile_.light.environmentLights.clear();

		for (auto& light : lights)
		{
			if (camera.getLayer() != light->getLayer())
				continue;

			if (light->getVisible())
			{
				light->onRenderBefore(camera);

				if (light->isA<light::AmbientLight>()) {
					this->profile_.light.ambientLightColors += light->getColor() * light->getIntensity();
				} else if (light->isA<light::DirectionalLight>()) {
					auto it = light->downcast<light::DirectionalLight>();
					DirectionalLight directionLight;
					directionLight.color.set(it->getColor() * it->getIntensity());
					directionLight.direction.set(it->getForward());
					directionLight.shadow = false;
					directionLight.shadowBias = it->getShadowBias();
					directionLight.shadowRadius = it->getShadowRadius();
					directionLight.shadowMapSize = math::float2::Zero;
					this->profile_.light.directionalLights.emplace_back(directionLight);
					this->profile_.light.numDirectional++;
				} else if (light->isA<light::SpotLight>()) {
					auto it = light->downcast<light::SpotLight>();
					SpotLight spotLight;
					spotLight.color.set(it->getColor() * it->getIntensity());
					spotLight.direction.set(it->getForward());
					spotLight.position.set(it->getTranslate());
					spotLight.distance = 0;
					spotLight.decay = 0;
					spotLight.coneCos = it->getInnerCone().y;
					spotLight.penumbraCos = it->getOuterCone().y;
					spotLight.shadow = false;
					spotLight.shadowBias = 0.0;
					spotLight.shadowRadius = it->getRange();
					spotLight.shadowMapSize = math::float2::Zero;
					this->profile_.light.spotLights.emplace_back(spotLight);
					this->profile_.light.numSpot++;
				} else if (light->isA<light::PointLight>()) {
					auto it = light->downcast<light::PointLight>();
					PointLight pointLight;
					pointLight.color.set(it->getColor() * it->getIntensity());
					pointLight.position.set(it->getTranslate());
					pointLight.distance = 0;
					pointLight.decay = 0;
					pointLight.shadow = false;
					pointLight.shadowBias = 0.0;
					pointLight.shadowRadius = it->getRange();
					pointLight.shadowMapSize = math::float2::Zero;
					this->profile_.light.pointLights.emplace_back(pointLight);
					this->profile_.light.numPoint++;
				} else if (light->isA<light::EnvironmentLight>()) {
					auto it = light->downcast<light::EnvironmentLight>();
					EnvironmentLight environmentLight;
					environmentLight.intensity = it->getIntensity();
					environmentLight.radiance = it->getEnvironmentMap();
					if (!it->getEnvironmentMap())
						this->profile_.light.ambientLightColors += light->getColor() * light->getIntensity();
					this->profile_.light.environmentLights.emplace_back(environmentLight);
					this->profile_.light.numEnvironment++;
				} else if (light->isA<light::RectangleLight>()) {
					auto it = light->downcast<light::RectangleLight>();
					RectAreaLight rectangleLight;
					rectangleLight.color.set(it->getColor() * it->getIntensity());
					rectangleLight.position.set(it->getTranslate());
					rectangleLight.halfWidth.set(math::float3::One);
					rectangleLight.halfHeight.set(math::float3::One);
					this->profile_.light.rectangleLights.emplace_back(rectangleLight);
					this->profile_.light.numRectangle++;
				}

				light->onRenderAfter(camera);

				this->lights_.emplace_back(light);
			}
		}

		if (this->profile_.light.numSpot)
		{
			if (!this->profile_.light.spotLightBuffer)
			{
				this->profile_.light.spotLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					this->profile_.light.spotLights.data(),
					sizeof(SpotLight) * this->profile_.light.numSpot
				));
			}
			else
			{
				auto desc = this->profile_.light.spotLightBuffer->getDataDesc();
				if (desc.getStreamSize() < this->profile_.light.spotLights.size() * sizeof(SpotLight))
				{
					this->profile_.light.spotLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						this->profile_.light.spotLights.data(),
						sizeof(SpotLight) * this->profile_.light.numSpot
					));
				}
				else
				{
					void* data;
					if (this->profile_.light.spotLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, this->profile_.light.spotLights.data(), this->profile_.light.spotLights.size() * sizeof(SpotLight));
					this->profile_.light.spotLightBuffer->unmap();
				}
			}
		}

		if (this->profile_.light.numPoint)
		{
			if (!this->profile_.light.pointLightBuffer)
			{
				this->profile_.light.pointLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					this->profile_.light.pointLights.data(),
					sizeof(PointLight) * this->profile_.light.numPoint
				));
			}
			else
			{
				auto desc = this->profile_.light.pointLightBuffer->getDataDesc();
				if (desc.getStreamSize() < this->profile_.light.pointLights.size() * sizeof(PointLight))
				{
					this->profile_.light.pointLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						this->profile_.light.pointLights.data(),
						sizeof(PointLight) * this->profile_.light.numPoint
					));
				}
				else
				{
					void* data;
					if (this->profile_.light.pointLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, this->profile_.light.pointLights.data(), this->profile_.light.pointLights.size() * sizeof(PointLight));
					this->profile_.light.pointLightBuffer->unmap();
				}
			}
		}

		if (this->profile_.light.numRectangle)
		{
			if (!this->profile_.light.rectangleLightBuffer)
			{
				this->profile_.light.rectangleLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					this->profile_.light.rectangleLights.data(),
					sizeof(RectAreaLight) * this->profile_.light.numRectangle
				));
			}
			else
			{
				auto desc = this->profile_.light.rectangleLightBuffer->getDataDesc();
				if (desc.getStreamSize() < this->profile_.light.rectangleLights.size() * sizeof(RectAreaLight))
				{
					this->profile_.light.rectangleLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						this->profile_.light.rectangleLights.data(),
						sizeof(RectAreaLight) * this->profile_.light.numRectangle
					));
				}
				else
				{
					void* data;
					if (this->profile_.light.rectangleLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, this->profile_.light.rectangleLights.data(), this->profile_.light.rectangleLights.size() * sizeof(RectAreaLight));
					this->profile_.light.rectangleLightBuffer->unmap();
				}
			}
		}

		if (this->profile_.light.numDirectional)
		{
			if (!this->profile_.light.directionLightBuffer)
			{
				this->profile_.light.directionLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					this->profile_.light.directionalLights.data(),
					sizeof(DirectionalLight) * this->profile_.light.numDirectional
				));
			}
			else
			{
				auto desc = this->profile_.light.directionLightBuffer->getDataDesc();
				if (desc.getStreamSize() < this->profile_.light.directionalLights.size() * sizeof(DirectionalLight))
				{
					this->profile_.light.directionLightBuffer = this->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						this->profile_.light.directionalLights.data(),
						sizeof(DirectionalLight) * this->profile_.light.numDirectional
					));
				}
				else
				{
					void* data;
					if (this->profile_.light.directionLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, this->profile_.light.directionalLights.data(), this->profile_.light.directionalLights.size() * sizeof(DirectionalLight));
					this->profile_.light.directionLightBuffer->unmap();
				}
			}
		}
	}

	void
	Renderer::setViewport(const math::float4& viewport) noexcept(false)
	{
		this->context_->setViewport(0, viewport);
	}

	void
	Renderer::setFramebuffer(const hal::GraphicsFramebufferPtr& framebuffer) noexcept(false)
	{
		this->context_->setFramebuffer(framebuffer);
	}

	void
	Renderer::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
	{
		this->context_->clearFramebuffer(i, flags, color, depth, stencil);
	}

	void
	Renderer::generateMipmap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		this->context_->generateMipmap(texture);
	}

	void
	Renderer::renderObject(const geometry::Geometry& geometry, const camera::Camera& camera) noexcept
	{
		if (camera.getLayer() != geometry.getLayer())
			return;

		if (geometry.getVisible())
		{
			if (!this->setProgram(this->overrideMaterial_ ? this->overrideMaterial_ : geometry.getMaterial(), camera, geometry))
				return;

			if (!this->setBuffer(geometry.getMesh(), geometry.getMeshSubset()))
				return;

			auto indices = currentBuffer_->getNumIndices(geometry.getMeshSubset());
			if (indices > 0)
				this->context_->drawIndexed((std::uint32_t)indices, 1, 0, 0, 0);
			else
				this->context_->draw((std::uint32_t)currentBuffer_->getNumVertices(), 1, 0, 0);
		}
	}

	void
	Renderer::renderObjects(const std::vector<geometry::Geometry*>& geometries, const camera::Camera& camera) noexcept
	{
		for (auto& geometry : geometries)
			this->renderObject(*geometry, camera);
	}

	void
	Renderer::render(RenderScene& scene) noexcept
	{
		for (auto& camera : scene.getCameraList())
		{
			camera->onRenderBefore(*camera);

			for (auto& it : scene.getGeometries())
			{
				if (camera->getLayer() != it->getLayer())
					continue;

				if (it->getVisible())
					it->onRenderBefore(*camera);
			}
		}

		if (this->sortObjects_)
		{
			scene.sortCameras();
			scene.sortGeometries();
		}

		for (auto& camera : scene.getCameraList())
		{
#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			auto framebuffer = camera->getFramebuffer();
			if (framebuffer)
				this->context_->setFramebuffer(framebuffer);
			else
				this->context_->setFramebuffer(fbo_);
#endif

			this->context_->setViewport(0, camera->getPixelViewport());
			this->context_->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);

			this->prepareLights(scene.getLights(), *camera);
			this->renderObjects(scene.getGeometries(), *camera);

#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			if (camera->getBlitToScreen())
			{
				auto& v = camera->getPixelViewport();
				if (framebuffer)
					this->context_->blitFramebuffer(framebuffer, v, nullptr, v);
				else
					this->context_->blitFramebuffer(fbo_, v, nullptr, v);
			}

			if (framebuffer)
			{
				auto& swapFramebuffer = camera->getSwapFramebuffer();
				if (swapFramebuffer)
				{
					math::float4 v1(0, 0, (float)framebuffer->getGraphicsFramebufferDesc().getWidth(), (float)framebuffer->getGraphicsFramebufferDesc().getHeight());
					math::float4 v2(0, 0, (float)swapFramebuffer->getGraphicsFramebufferDesc().getWidth(), (float)swapFramebuffer->getGraphicsFramebufferDesc().getHeight());
					this->context_->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
				}
			}
#endif
		}

		for (auto& camera : scene.getCameraList())
		{
			for (auto& it : scene.getGeometries())
			{
				if (camera->getLayer() != it->getLayer())
					continue;

				if (it->getVisible())
					it->onRenderAfter(*camera);
			}

			camera->onRenderAfter(*camera);
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
	Renderer::setBuffer(const std::shared_ptr<mesh::Mesh>& mesh, std::size_t subset)
	{
		if (mesh)
		{
			auto& buffer = buffers_[((std::intptr_t)mesh.get())];
			if (!buffer)
				buffer = std::make_shared<RenderBuffer>(mesh);

			this->context_->setVertexBufferData(0, buffer->getVertexBuffer(), 0);
			this->context_->setIndexBufferData(buffer->getIndexBuffer(subset), 0, hal::GraphicsIndexType::UInt32);

			this->currentBuffer_ = buffer;

			return true;
		}
		else
		{
			return false;
		}
	}

	bool
	Renderer::setProgram(const std::shared_ptr<material::Material>& material, const camera::Camera& camera, const geometry::Geometry& geometry)
	{
		if (material)
		{
			auto& pipeline = pipelines_[((std::intptr_t)material.get())];
			if (!pipeline)
				pipeline = std::make_shared<RenderPipeline>(material, this->profile_);

			pipeline->update(camera, geometry, this->profile_);

			this->context_->setRenderPipeline(pipeline->getPipeline());
			this->context_->setDescriptorSet(pipeline->getDescriptorSet());

			return true;
		}
		else
		{
			return false;
		}
	}
}