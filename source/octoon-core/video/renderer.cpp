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

#include <octoon/material/mesh_depth_material.h>
#include <octoon/material/mesh_standard_material.h>

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
		depthMaterial_ = material::MeshDepthMaterial::create();
		montecarlo_ = std::make_shared<MonteCarlo>(w, h);

		this->setFramebufferSize(w, h);
	}

	void
	Renderer::close() noexcept
	{
		this->profile_.reset();
		this->buffers_.clear();
		this->pipelines_.clear();
		fbo_.reset();
		currentBuffer_.reset();
		colorTexture_.reset();
		depthTexture_.reset();
		renderer_.reset();
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
		this->renderer_ = context;
	}
	
	const hal::GraphicsContextPtr&
	Renderer::getGraphicsContext() const noexcept(false)
	{
		return this->renderer_;
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
	Renderer::setViewport(const math::float4& viewport) noexcept(false)
	{
		this->renderer_->setViewport(0, viewport);
	}

	void
	Renderer::setFramebuffer(const hal::GraphicsFramebufferPtr& framebuffer) noexcept(false)
	{
		this->renderer_->setFramebuffer(framebuffer);
	}

	void
	Renderer::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
	{
		this->renderer_->clearFramebuffer(i, flags, color, depth, stencil);
	}

	void
	Renderer::generateMipmap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		this->renderer_->generateMipmap(texture);
	}

	void
	Renderer::renderObject(const geometry::Geometry& geometry, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial) noexcept
	{
		if (camera.getLayer() != geometry.getLayer())
			return;

		if (geometry.getVisible())
		{
			for (std::size_t i = 0; i < geometry.getMaterials().size(); i++)
			{
				if (!this->setProgram(overrideMaterial ? overrideMaterial : geometry.getMaterials()[i], camera, geometry))
					return;

				if (!this->setBuffer(geometry.getMesh(), i))
					return;

				auto indices = currentBuffer_->getNumIndices(i);
				if (indices > 0)
					this->renderer_->drawIndexed((std::uint32_t)indices, 1, 0, 0, 0);
				else
					this->renderer_->draw((std::uint32_t)currentBuffer_->getNumVertices(), 1, 0, 0);
			}
		}
	}

	void
	Renderer::renderObjects(const std::vector<geometry::Geometry*>& geometries, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial) noexcept
	{
		for (auto& geometry : geometries)
			this->renderObject(*geometry, camera, overrideMaterial);
	}

	void
	Renderer::prepareShadowMaps(const std::vector<light::Light*>& lights, const std::vector<geometry::Geometry*>& geometries) noexcept
	{
		for (auto& light : lights)
		{
			if (!light->getVisible())
				continue;

			std::uint32_t faceCount = 0;
			std::shared_ptr<camera::Camera> camera;

			if (light->isA<light::DirectionalLight>())
			{
				auto directionalLight = light->cast<light::DirectionalLight>();
				if (directionalLight->getShadowEnable())
				{
					faceCount = 1;
					camera = directionalLight->getCamera();
				}
			}
			else if (light->isA<light::SpotLight>())
			{
				auto spotLight = light->cast<light::SpotLight>();
				if (spotLight->getShadowEnable())
				{
					faceCount = 1;
					camera = spotLight->getCamera();
				}
			}
			else if (light->isA<light::PointLight>())
			{
				auto pointLight = light->cast<light::PointLight>();
				if (pointLight->getShadowEnable())
				{
					faceCount = 6;
					camera = pointLight->getCamera();
				}
			}

			for (std::uint32_t face = 0; face < faceCount; face++)
			{
				auto framebuffer = camera->getFramebuffer();

				this->renderer_->setFramebuffer(framebuffer ? framebuffer : fbo_);
				this->renderer_->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
				this->renderer_->setViewport(0, camera->getPixelViewport());

				this->prepareLights(*camera, lights);

				for (auto& geometry : geometries)
				{
					if (geometry->getMaterial()->getPrimitiveType() == this->depthMaterial_->getPrimitiveType())
						this->renderObject(*geometry, *camera, this->depthMaterial_);
				}

				if (camera->getRenderToScreen())
				{
					auto& v = camera->getPixelViewport();
					this->renderer_->blitFramebuffer(framebuffer ? framebuffer : fbo_, v, nullptr, v);
				}

				auto& swapFramebuffer = camera->getSwapFramebuffer();
				if (swapFramebuffer && framebuffer)
				{
					math::float4 v1(0, 0, (float)framebuffer->getFramebufferDesc().getWidth(), (float)framebuffer->getFramebufferDesc().getHeight());
					math::float4 v2(0, 0, (float)swapFramebuffer->getFramebufferDesc().getWidth(), (float)swapFramebuffer->getFramebufferDesc().getHeight());
					this->renderer_->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
				}
			}
		}
	}

	void
	Renderer::prepareLights(const camera::Camera& camera, const std::vector<light::Light*>& lights) noexcept
	{
		this->profile_.reset();

		for (auto& light : lights)
		{
			if (camera.getLayer() != light->getLayer())
				continue;

			if (light->getVisible())
			{
				light->onRenderBefore(camera);

				if (light->isA<light::AmbientLight>())
				{
					this->profile_.light.ambientLightColors += light->getColor() * light->getIntensity() * math::PI;
				}
				else if (light->isA<light::EnvironmentLight>())
				{
					auto it = light->downcast<light::EnvironmentLight>();
					EnvironmentLight environmentLight;
					environmentLight.intensity = it->getIntensity();
					environmentLight.radiance = it->getEnvironmentMap();
					if (!it->getEnvironmentMap())
						this->profile_.light.ambientLightColors += light->getColor() * light->getIntensity() * math::PI;
					this->profile_.light.environmentLights.emplace_back(environmentLight);
					this->profile_.light.numEnvironment++;
				}
				else if (light->isA<light::DirectionalLight>())
				{
					auto it = light->downcast<light::DirectionalLight>();
					DirectionalLight directionLight;
					directionLight.direction = math::float4(float3x3(camera.getView()) * -it->getForward(), 0);
					directionLight.color = it->getColor() * it->getIntensity();
					directionLight.shadow = it->getShadowEnable();

					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer && directionLight.shadow)
					{
						directionLight.shadow = it->getShadowEnable();
						directionLight.shadowBias = it->getShadowBias();
						directionLight.shadowRadius = it->getShadowRadius();
						directionLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						math::float4x4 viewport;
						viewport.makeScale(math::float3(0.5f, 0.5f, 0.5f));
						viewport.translate(math::float3(0.5f, 0.5f, 0.5f));

						this->profile_.light.directionalShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
						this->profile_.light.directionalShadowMatrix.push_back(viewport * it->getCamera()->getViewProjection());
					}

					this->profile_.light.numDirectional++;
					this->profile_.light.directionalLights.emplace_back(directionLight);
				}
				else if (light->isA<light::SpotLight>())
				{
					auto it = light->downcast<light::SpotLight>();
					SpotLight spotLight;
					spotLight.color.set(it->getColor() * it->getIntensity());
					spotLight.direction.set(float3x3(camera.getView()) * it->getForward());
					spotLight.position.set(it->getTranslate());
					spotLight.distance = 0;
					spotLight.decay = 0;
					spotLight.coneCos = it->getInnerCone().y;
					spotLight.penumbraCos = it->getOuterCone().y;
					spotLight.shadow = it->getShadowEnable();

					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer && spotLight.shadow)
					{						
						spotLight.shadowBias = it->getShadowBias();
						spotLight.shadowRadius = it->getShadowRadius();
						spotLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						this->profile_.light.spotShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					}

					this->profile_.light.spotLights.emplace_back(spotLight);
					this->profile_.light.numSpot++;
				}
				else if (light->isA<light::PointLight>())
				{
					auto it = light->downcast<light::PointLight>();
					PointLight pointLight;
					pointLight.color.set(it->getColor() * it->getIntensity());
					pointLight.position.set(it->getTranslate());
					pointLight.distance = 0;
					pointLight.decay = 0;
					pointLight.shadow = it->getShadowEnable();

					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer && pointLight.shadow)
					{
						pointLight.shadowBias = it->getShadowBias();
						pointLight.shadowRadius = it->getShadowRadius();
						pointLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						this->profile_.light.pointShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					}

					this->profile_.light.pointLights.emplace_back(pointLight);
					this->profile_.light.numPoint++;
				}
				else if (light->isA<light::RectangleLight>())
				{
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
	Renderer::prepareLightMaps(const camera::Camera& camera, const std::vector<light::Light*>& lights, const std::vector<geometry::Geometry*>& geometries) noexcept
	{
		for (auto& geometry : geometries)
		{
			if (camera.getLayer() != geometry->getLayer())
				return;

			if (geometry->getVisible() && geometry->getGlobalIllumination())
			{
				auto& lightmap = lightmaps_[((std::intptr_t)geometry->getMesh().get())];
				if (!lightmap)
				{
					lightmap = std::make_shared<bake::Lightmap>();
					lightmap->setGeometry(*geometry);
				}

				for (auto& light : lights)
				{
					if (camera.getLayer() != light->getLayer())
						continue;

					if (light->getVisible())
					{
						if (light->isA<light::DirectionalLight>())
							lightmap->renderLight(*light->downcast<light::DirectionalLight>());
					}
				}

				lightmap->render(camera);

				auto& texture = lightTextures_[((std::intptr_t)geometry->getMesh().get())];
				if (!texture)
				{
					GraphicsTextureDesc lightMapDesc;
					lightMapDesc.setWidth(lightmap->width());
					lightMapDesc.setHeight(lightmap->height());
					lightMapDesc.setTexDim(GraphicsTextureDim::Texture2D);
					lightMapDesc.setTexFormat(GraphicsFormat::R32G32B32SFloat);
					lightMapDesc.setUsageFlagBits(GraphicsUsageFlagBits::WriteBit);
					lightMapDesc.setStream(lightmap->fronBuffer().data());
					lightMapDesc.setStreamSize(lightmap->fronBuffer().size() * sizeof(math::float3));

					texture = device_->createTexture(lightMapDesc);

					for (auto& it : geometry->getMaterials())
					{
						if (it->isA<material::MeshStandardMaterial>())
						{
							auto material = it->downcast< material::MeshStandardMaterial>();
							material->setLightMap(texture);
						}
					}
				}
				else
				{
					void* data;
					if (texture->map(0, 0, texture->getTextureDesc().getWidth(), texture->getTextureDesc().getHeight(), 0, &data))
					{
						std::memcpy(data, lightmap->fronBuffer().data(), lightmap->fronBuffer().size() * sizeof(math::float3));
						texture->unmap();
					}
				}
			}
		}
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

		this->prepareShadowMaps(scene.getLights(), scene.getGeometries());

		for (auto& camera : scene.getCameraList())
		{
			/*this->montecarlo_->render(
				*camera,
				scene.getLights(),
				scene.getGeometries(),
				0,
				camera->getPixelViewport().x,
				camera->getPixelViewport().y,
				camera->getPixelViewport().width,
				camera->getPixelViewport().height);*/

			auto framebuffer = camera->getFramebuffer();
			this->renderer_->setFramebuffer(framebuffer ? framebuffer : fbo_);
			this->renderer_->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
			this->renderer_->setViewport(0, camera->getPixelViewport());

			this->prepareLights(*camera, scene.getLights());
			this->renderObjects(scene.getGeometries(), *camera, this->overrideMaterial_);

			if (camera->getRenderToScreen())
			{
				auto& v = camera->getPixelViewport();
				this->renderer_->blitFramebuffer(framebuffer ? framebuffer : fbo_, v, nullptr, v);
			}

			auto& swapFramebuffer = camera->getSwapFramebuffer();
			if (framebuffer && swapFramebuffer)
			{
				math::float4 v1(0, 0, (float)framebuffer->getFramebufferDesc().getWidth(), (float)framebuffer->getFramebufferDesc().getHeight());
				math::float4 v2(0, 0, (float)swapFramebuffer->getFramebufferDesc().getWidth(), (float)swapFramebuffer->getFramebufferDesc().getHeight());
				this->renderer_->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
			}
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

			this->renderer_->setVertexBufferData(0, buffer->getVertexBuffer(), 0);
			this->renderer_->setIndexBufferData(buffer->getIndexBuffer(subset), 0, hal::GraphicsIndexType::UInt32);

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

			this->renderer_->setRenderPipeline(pipeline->getPipeline());
			this->renderer_->setDescriptorSet(pipeline->getDescriptorSet());

			return true;
		}
		else
		{
			return false;
		}
	}
}