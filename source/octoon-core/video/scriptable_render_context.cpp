#include <octoon/video/scriptable_render_context.h>
#include <octoon/video/scriptable_render_buffer.h>
#include <octoon/video/scriptable_render_material.h>
#include <octoon/video/rendering_data.h>

#include <octoon/camera/perspective_camera.h>
#include <octoon/light/ambient_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/disk_light.h>
#include <octoon/light/rectangle_light.h>
#include <octoon/light/environment_light.h>
#include <octoon/light/tube_light.h>

#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_framebuffer.h>
#include <octoon/hal/graphics_data.h>
#include <octoon/hal/graphics_context.h>

namespace octoon
{
	ScriptableRenderContext::ScriptableRenderContext()
	{
	}

	ScriptableRenderContext::ScriptableRenderContext(const hal::GraphicsContextPtr& context)
		: context_(context)
	{
	}

	ScriptableRenderContext::~ScriptableRenderContext()
	{
	}

	void
	ScriptableRenderContext::setGraphicsContext(const hal::GraphicsContextPtr& context) noexcept(false)
	{
		this->context_ = context;
	}

	const hal::GraphicsContextPtr&
	ScriptableRenderContext::getGraphicsContext() const noexcept(false)
	{
		return this->context_;
	}

	hal::GraphicsInputLayoutPtr
	ScriptableRenderContext::createInputLayout(const hal::GraphicsInputLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createInputLayout(desc);
	}

	hal::GraphicsDataPtr
	ScriptableRenderContext::createGraphicsData(const hal::GraphicsDataDesc& desc) noexcept
	{
		return this->context_->getDevice()->createGraphicsData(desc);
	}

	hal::GraphicsTexturePtr
	ScriptableRenderContext::createTexture(const hal::GraphicsTextureDesc& desc) noexcept
	{
		return this->context_->getDevice()->createTexture(desc);
	}

	hal::GraphicsSamplerPtr
	ScriptableRenderContext::createSampler(const hal::GraphicsSamplerDesc& desc) noexcept
	{
		return this->context_->getDevice()->createSampler(desc);
	}

	hal::GraphicsFramebufferPtr
	ScriptableRenderContext::createFramebuffer(const hal::GraphicsFramebufferDesc& desc) noexcept
	{
		return this->context_->getDevice()->createFramebuffer(desc);
	}

	hal::GraphicsFramebufferLayoutPtr
	ScriptableRenderContext::createFramebufferLayout(const hal::GraphicsFramebufferLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createFramebufferLayout(desc);
	}

	hal::GraphicsShaderPtr
	ScriptableRenderContext::createShader(const hal::GraphicsShaderDesc& desc) noexcept
	{
		return this->context_->getDevice()->createShader(desc);
	}

	hal::GraphicsProgramPtr
	ScriptableRenderContext::createProgram(const hal::GraphicsProgramDesc& desc) noexcept
	{
		return this->context_->getDevice()->createProgram(desc);
	}

	hal::GraphicsStatePtr
	ScriptableRenderContext::createRenderState(const hal::GraphicsStateDesc& desc) noexcept
	{
		return this->context_->getDevice()->createRenderState(desc);
	}

	hal::GraphicsPipelinePtr
	ScriptableRenderContext::createRenderPipeline(const hal::GraphicsPipelineDesc& desc) noexcept
	{
		return this->context_->getDevice()->createRenderPipeline(desc);
	}

	hal::GraphicsDescriptorSetPtr
	ScriptableRenderContext::createDescriptorSet(const hal::GraphicsDescriptorSetDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorSet(desc);
	}

	hal::GraphicsDescriptorSetLayoutPtr
	ScriptableRenderContext::createDescriptorSetLayout(const hal::GraphicsDescriptorSetLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorSetLayout(desc);
	}

	hal::GraphicsDescriptorPoolPtr
	ScriptableRenderContext::createDescriptorPool(const hal::GraphicsDescriptorPoolDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorPool(desc);
	}

	void
	ScriptableRenderContext::setViewport(std::uint32_t i, const math::float4& viewport) noexcept
	{
		this->context_->setViewport(i, viewport);
	}

	const math::float4&
	ScriptableRenderContext::getViewport(std::uint32_t i) const noexcept
	{
		return this->context_->getViewport(i);
	}

	void
	ScriptableRenderContext::setScissor(std::uint32_t i, const math::uint4& scissor) noexcept
	{
		this->context_->setScissor(i, scissor);
	}

	const math::uint4&
	ScriptableRenderContext::getScissor(std::uint32_t i) const noexcept
	{
		return this->context_->getScissor(i);
	}

	void
	ScriptableRenderContext::setStencilCompareMask(hal::GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
	{
		this->context_->setStencilCompareMask(face, mask);
	}
	std::uint32_t
	ScriptableRenderContext::getStencilCompareMask(hal::GraphicsStencilFaceFlags face) noexcept
	{
		return this->context_->getStencilCompareMask(face);
	}

	void
	ScriptableRenderContext::setStencilReference(hal::GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
	{
		this->context_->setStencilReference(face, reference);
	}

	std::uint32_t
	ScriptableRenderContext::getStencilReference(hal::GraphicsStencilFaceFlags face) noexcept
	{
		return this->context_->getStencilReference(face);
	}

	void
	ScriptableRenderContext::setStencilWriteMask(hal::GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
	{
		this->context_->setStencilWriteMask(face, mask);
	}

	std::uint32_t
	ScriptableRenderContext::getStencilWriteMask(hal::GraphicsStencilFaceFlags face) noexcept
	{
		return this->context_->getStencilWriteMask(face);
	}

	void
	ScriptableRenderContext::setRenderPipeline(const hal::GraphicsPipelinePtr& pipeline) noexcept
	{
		this->context_->setRenderPipeline(pipeline);
	}

	hal::GraphicsPipelinePtr
	ScriptableRenderContext::getRenderPipeline() const noexcept
	{
		return this->context_->getRenderPipeline();
	}

	void
	ScriptableRenderContext::setDescriptorSet(const hal::GraphicsDescriptorSetPtr& descriptorSet) noexcept
	{
		this->context_->setDescriptorSet(descriptorSet);
	}

	hal::GraphicsDescriptorSetPtr
	ScriptableRenderContext::getDescriptorSet() const noexcept
	{
		return this->context_->getDescriptorSet();
	}

	void
	ScriptableRenderContext::setVertexBufferData(std::uint32_t i, const hal::GraphicsDataPtr& data, std::intptr_t offset) noexcept
	{
		this->context_->setVertexBufferData(i, data, offset);
	}

	hal::GraphicsDataPtr
	ScriptableRenderContext::getVertexBufferData(std::uint32_t i) const noexcept
	{
		return this->context_->getVertexBufferData(i);
	}

	void
	ScriptableRenderContext::setIndexBufferData(const hal::GraphicsDataPtr& data, std::intptr_t offset, hal::GraphicsIndexType indexType) noexcept
	{
		this->context_->setIndexBufferData(data, offset, indexType);
	}

	hal::GraphicsDataPtr
	ScriptableRenderContext::getIndexBufferData() const noexcept
	{
		return this->context_->getIndexBufferData();
	}

	void
	ScriptableRenderContext::configureTarget(const hal::GraphicsFramebufferPtr& target) noexcept
	{
		this->context_->setFramebuffer(target);
	}

	void
	ScriptableRenderContext::configureClear(hal::GraphicsClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept
	{
		this->context_->clearFramebuffer(0, flags, color, depth, stencil);
	}

	void
	ScriptableRenderContext::discardFramebuffer(const hal::GraphicsFramebufferPtr& src, hal::GraphicsClearFlags flags) noexcept
	{
		this->context_->discardFramebuffer(src, flags);
	}

	void
	ScriptableRenderContext::blitFramebuffer(const hal::GraphicsFramebufferPtr& src, const math::float4& v1, const hal::GraphicsFramebufferPtr& dest, const math::float4& v2) noexcept
	{
		this->context_->blitFramebuffer(src, v1, dest, v2);
	}

	void
	ScriptableRenderContext::readFramebuffer(std::uint32_t i, const hal::GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
	{
		this->context_->readFramebuffer(i, texture, miplevel, x, y, width, height);
	}

	void
	ScriptableRenderContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const hal::GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
	{
		this->context_->readFramebufferToCube(i, face, texture, miplevel, x, y, width, height);
	}

	hal::GraphicsFramebufferPtr
	ScriptableRenderContext::getFramebuffer() const noexcept
	{
		return this->context_->getFramebuffer();
	}

	void
	ScriptableRenderContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
	{
		this->context_->draw(numVertices, numInstances, startVertice, startInstances);
	}

	void
	ScriptableRenderContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
	{
		this->context_->drawIndexed(numIndices, numInstances, startIndice, startVertice, startInstances);
	}

	void
	ScriptableRenderContext::drawIndirect(const hal::GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
	{
		this->context_->drawIndirect(data, offset, drawCount, stride);
	}

	void
	ScriptableRenderContext::drawIndexedIndirect(const hal::GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
	{
		this->context_->drawIndexedIndirect(data, offset, drawCount, stride);
	}

	void
	ScriptableRenderContext::compileScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		materialCollector.Clear();

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible())
				continue;

			for (std::size_t i = 0; i < geometry->getMaterials().size(); ++i)
			{
				auto& mat = geometry->getMaterial(i);
				materialCollector.Collect(mat);
			}
		}

		materialCollector.Commit();

		if (!renderingData_)
		{
			auto out = std::make_unique<RenderingData>();
			this->updateCamera(scene, *out, true);
			this->updateLights(scene, *out, true);
			this->updateMaterials(scene, *out, true);
			this->updateShapes(scene, *out, true);
			renderingData_ = std::move(out);
		}
		else
		{
			auto& out = *renderingData_;

			bool should_update_lights = false;
			for (auto& light : scene->getLights())
			{
				if (light->isDirty())
				{
					should_update_lights = true;
					break;
				}
			}

			bool should_update_shapes = false;
			for (auto& geometry : scene->getGeometries())
			{
				if (!geometry->getVisible())
					continue;

				if (geometry->isDirty())
				{
					should_update_shapes = true;
					break;
				}
			}

			bool should_update_materials = !out.material_bundle || materialCollector.NeedsUpdate(out.material_bundle.get(),
				[](runtime::RttiInterface* ptr)->bool
			{
				auto mat = ptr->downcast<Material>();
				return mat->isDirty();
			});

			auto camera = scene->getMainCamera();
			bool should_update_camera = out.camera != camera || camera->isDirty();

			if (should_update_camera)
				this->updateCamera(scene, out);
			
			if (should_update_lights || should_update_camera)
				this->updateLights(scene, out);

			if (should_update_materials)
				this->updateMaterials(scene, out);

			if (should_update_shapes)
				this->updateShapes(scene, out);
		}
	}

	void
	ScriptableRenderContext::cleanCache() noexcept
	{
		renderingData_.reset();
	}

	RenderingData&
	ScriptableRenderContext::getRenderingData() const noexcept(false)
	{
		return *renderingData_;
	}

	void
	ScriptableRenderContext::updateCamera(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
	{
		out.camera = scene->getMainCamera();
	}

	void
	ScriptableRenderContext::updateLights(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
	{
		out.reset();

		for (auto& light : scene->getLights())
		{
			if (!light->getVisible())
				continue;

			if (light->getLayer() != out.camera->getLayer())
				continue;
			
			light->onRenderBefore(*out.camera);

			if (light->isA<AmbientLight>())
			{
				out.ambientLightColors += light->getColor() * light->getIntensity();
			}
			else if (light->isA<EnvironmentLight>())
			{
				auto it = light->downcast<EnvironmentLight>();
				RenderingData::EnvironmentLight environmentLight;
				environmentLight.intensity = it->getIntensity();
				environmentLight.offset = it->getOffset();
				environmentLight.radiance = it->getEnvironmentMap();
				if (!it->getEnvironmentMap())
					out.ambientLightColors += light->getColor() * light->getIntensity();
				out.environmentLights.emplace_back(environmentLight);
				out.numEnvironment++;
			}
			else if (light->isA<DirectionalLight>())
			{
				auto it = light->downcast<DirectionalLight>();
				auto color = it->getColor() * it->getIntensity();
				RenderingData::DirectionalLight directionLight;
				directionLight.direction = math::float4(math::float3x3(out.camera->getView()) * -it->getForward(), 0);
				directionLight.color[0] = color.x;
				directionLight.color[1] = color.y;
				directionLight.color[2] = color.z;
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

					out.directionalShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					out.directionalShadowMatrix.push_back(viewport * it->getCamera()->getViewProjection());
				}

				out.numDirectional++;
				out.directionalLights.emplace_back(directionLight);
			}
			else if (light->isA<SpotLight>())
			{
				auto it = light->downcast<SpotLight>();
				RenderingData::SpotLight spotLight;
				spotLight.color.set(it->getColor() * it->getIntensity());
				spotLight.direction.set(math::float3x3(out.camera->getView()) * it->getForward());
				spotLight.position.set(it->getTranslate());
				spotLight.distance = 0;
				spotLight.decay = 0;
				spotLight.coneCos = it->getInnerCone().y;
				spotLight.penumbraCos = it->getOuterCone().y;
				spotLight.shadow = it->getShadowEnable();

				if (spotLight.shadow)
				{
					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer)
					{
						spotLight.shadowBias = it->getShadowBias();
						spotLight.shadowRadius = it->getShadowRadius();
						spotLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						out.spotShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					}
				}

				out.spotLights.emplace_back(spotLight);
				out.numSpot++;
			}
			else if (light->isA<PointLight>())
			{
				auto it = light->downcast<PointLight>();
				RenderingData::PointLight pointLight;
				pointLight.color.set(it->getColor() * it->getIntensity());
				pointLight.position.set(it->getTranslate());
				pointLight.distance = 0;
				pointLight.decay = 0;
				pointLight.shadow = it->getShadowEnable();

				if (pointLight.shadow)
				{
					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer && pointLight.shadow)
					{
						pointLight.shadowBias = it->getShadowBias();
						pointLight.shadowRadius = it->getShadowRadius();
						pointLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						out.pointShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					}
				}

				out.pointLights.emplace_back(pointLight);
				out.numPoint++;
			}
			else if (light->isA<RectangleLight>())
			{
				auto it = light->downcast<RectangleLight>();
				RenderingData::RectAreaLight rectangleLight;
				rectangleLight.color.set(it->getColor() * it->getIntensity());
				rectangleLight.position.set(it->getTranslate());
				rectangleLight.halfWidth.set(math::float3::One);
				rectangleLight.halfHeight.set(math::float3::One);
				out.rectangleLights.emplace_back(rectangleLight);
				out.numRectangle++;
			}

			light->onRenderAfter(*out.camera);

			out.lights.push_back(light);
		}

		if (out.numSpot)
		{
			if (!out.spotLightBuffer)
			{
				out.spotLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.spotLights.data(),
					sizeof(RenderingData::SpotLight) * out.numSpot
				));
			}
			else
			{
				auto desc = out.spotLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.spotLights.size() * sizeof(RenderingData::SpotLight))
				{
					out.spotLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.spotLights.data(),
						sizeof(RenderingData::SpotLight) * out.numSpot
					));
				}
				else
				{
					void* data;
					if (out.spotLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.spotLights.data(), out.spotLights.size() * sizeof(RenderingData::SpotLight));
					out.spotLightBuffer->unmap();
				}
			}
		}

		if (out.numPoint)
		{
			if (!out.pointLightBuffer)
			{
				out.pointLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.pointLights.data(),
					sizeof(RenderingData::PointLight) * out.numPoint
				));
			}
			else
			{
				auto desc = out.pointLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.pointLights.size() * sizeof(RenderingData::PointLight))
				{
					out.pointLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.pointLights.data(),
						sizeof(RenderingData::PointLight) * out.numPoint
					));
				}
				else
				{
					void* data;
					if (out.pointLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.pointLights.data(), out.pointLights.size() * sizeof(RenderingData::PointLight));
					out.pointLightBuffer->unmap();
				}
			}
		}

		if (out.numRectangle)
		{
			if (!out.rectangleLightBuffer)
			{
				out.rectangleLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.rectangleLights.data(),
					sizeof(RenderingData::RectAreaLight) * out.numRectangle
				));
			}
			else
			{
				auto desc = out.rectangleLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.rectangleLights.size() * sizeof(RenderingData::RectAreaLight))
				{
					out.rectangleLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.rectangleLights.data(),
						sizeof(RenderingData::RectAreaLight) * out.numRectangle
					));
				}
				else
				{
					void* data;
					if (out.rectangleLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.rectangleLights.data(), out.rectangleLights.size() * sizeof(RenderingData::RectAreaLight));
					out.rectangleLightBuffer->unmap();
				}
			}
		}

		if (out.numDirectional)
		{
			if (!out.directionLightBuffer)
			{
				out.directionLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.directionalLights.data(),
					sizeof(RenderingData::DirectionalLight) * out.numDirectional
				));
			}
			else
			{
				auto desc = out.directionLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.directionalLights.size() * sizeof(RenderingData::DirectionalLight))
				{
					out.directionLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.directionalLights.data(),
						sizeof(RenderingData::DirectionalLight) * out.numDirectional
					));
				}
				else
				{
					void* data;
					if (out.directionLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.directionalLights.data(), out.directionalLights.size() * sizeof(RenderingData::DirectionalLight));
					out.directionLightBuffer->unmap();
				}
			}
		}
	}

	void
	ScriptableRenderContext::updateMaterials(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
	{
		out.material_bundle.reset(materialCollector.CreateBundle());

		if (out.depthMaterial->isDirty())
		{
			this->materials_[out.depthMaterial.get()] = std::make_shared<ScriptableRenderMaterial>(*this, out.depthMaterial, out);
			out.depthMaterial->setDirty(false);
		}

		if (out.overrideMaterial)
		{
			this->materials_[out.depthMaterial.get()] = std::make_shared<ScriptableRenderMaterial>(*this, out.depthMaterial, out);
			out.overrideMaterial->setDirty(false);
		}

		std::unique_ptr<Iterator> mat_iter(materialCollector.CreateIterator());
		for (std::size_t i = 0; mat_iter->IsValid(); mat_iter->Next(), i++)
		{
			auto mat = mat_iter->ItemAs<Material>();
			if (mat->isDirty() || force)
			{
				auto material = mat->downcast_pointer<Material>();
				this->materials_[material.get()] = std::make_shared<ScriptableRenderMaterial>(*this, material, out);
			}
		}
	}

    void
    ScriptableRenderContext::updateShapes(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
    {
		out.geometries = scene->getGeometries();

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible())
				continue;

			if (geometry->isDirty() || force)
			{
				auto mesh = geometry->getMesh();
				this->buffers_[mesh.get()] = std::make_shared<ScriptableRenderBuffer>(*this, mesh);
			}
		}
    }

	void
	ScriptableRenderContext::setMaterial(const std::shared_ptr<Material>& material, const Camera& camera, const Geometry& geometry)
	{
		assert(material);

		auto& pipeline = this->materials_.at(material.get());
		pipeline->update(*this->renderingData_, camera, geometry);

		this->setRenderPipeline(pipeline->getPipeline());
		this->setDescriptorSet(pipeline->getDescriptorSet());
	}

	void
	ScriptableRenderContext::generateMipmap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		this->context_->generateMipmap(texture);
	}

	void
	ScriptableRenderContext::drawMesh(const std::shared_ptr<Mesh>& mesh, std::size_t subset)
	{
		auto& buffer = buffers_.at(mesh.get());
		this->setVertexBufferData(0, buffer->getVertexBuffer(), 0);
		this->setIndexBufferData(buffer->getIndexBuffer(), 0, hal::GraphicsIndexType::UInt32);

		if (buffer->getIndexBuffer())
			this->drawIndexed((std::uint32_t)buffer->getNumIndices(subset), 1, (std::uint32_t)buffer->getStartIndices(subset), 0, 0);
		else
			this->draw((std::uint32_t)buffer->getNumVertices(), 1, 0, 0);
	}

	void
	ScriptableRenderContext::drawRenderers(const Geometry& geometry, const Camera& camera, const std::shared_ptr<Material>& overrideMaterial) noexcept
	{
		if (camera.getLayer() != geometry.getLayer())
			return;

		if (geometry.getVisible())
		{
			for (std::size_t i = 0; i < geometry.getMaterials().size(); i++)
			{
				auto mesh = geometry.getMesh();
				auto material = geometry.getMaterials()[i];
				if (material && overrideMaterial)
				{
					if (material->getPrimitiveType() == overrideMaterial->getPrimitiveType())
						material = overrideMaterial;
				}

				if (mesh && material)
				{
					this->setMaterial(overrideMaterial ? overrideMaterial : material, camera, geometry);
					this->drawMesh(mesh, i);
				}
			}
		}
	}

	void
	ScriptableRenderContext::drawRenderers(const std::vector<Geometry*>& geometries, const Camera& camera, const std::shared_ptr<Material>& overrideMaterial) noexcept
	{
		for (auto& geometry : geometries)
			this->drawRenderers(*geometry, camera, overrideMaterial);
	}
}
