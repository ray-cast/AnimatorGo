#include <octoon/video/scriptable_render_context.h>
#include <octoon/video/scriptable_render_buffer.h>
#include <octoon/video/scriptable_render_material.h>
#include <octoon/video/rendering_data.h>
#include <octoon/mesh/plane_mesh.h>
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

	ScriptableRenderContext::ScriptableRenderContext(const GraphicsContextPtr& context)
		: context_(context)
	{
	}

	ScriptableRenderContext::~ScriptableRenderContext()
	{
	}

	void
	ScriptableRenderContext::setGraphicsContext(const GraphicsContextPtr& context) noexcept(false)
	{
		this->context_ = context;
	}

	const GraphicsContextPtr&
	ScriptableRenderContext::getGraphicsContext() const noexcept(false)
	{
		return this->context_;
	}

	GraphicsInputLayoutPtr
	ScriptableRenderContext::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createInputLayout(desc);
	}

	GraphicsDataPtr
	ScriptableRenderContext::createGraphicsData(const GraphicsDataDesc& desc) noexcept
	{
		return this->context_->getDevice()->createGraphicsData(desc);
	}

	std::shared_ptr<GraphicsTexture>
	ScriptableRenderContext::createTexture(const GraphicsTextureDesc& desc) noexcept
	{
		return this->context_->getDevice()->createTexture(desc);
	}

	GraphicsSamplerPtr
	ScriptableRenderContext::createSampler(const GraphicsSamplerDesc& desc) noexcept
	{
		return this->context_->getDevice()->createSampler(desc);
	}

	GraphicsFramebufferPtr
	ScriptableRenderContext::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
	{
		return this->context_->getDevice()->createFramebuffer(desc);
	}

	GraphicsFramebufferLayoutPtr
	ScriptableRenderContext::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createFramebufferLayout(desc);
	}

	GraphicsShaderPtr
	ScriptableRenderContext::createShader(const GraphicsShaderDesc& desc) noexcept
	{
		return this->context_->getDevice()->createShader(desc);
	}

	GraphicsProgramPtr
	ScriptableRenderContext::createProgram(const GraphicsProgramDesc& desc) noexcept
	{
		return this->context_->getDevice()->createProgram(desc);
	}

	std::shared_ptr<RenderState>
	ScriptableRenderContext::createRenderState(const RenderStateDesc& desc) noexcept
	{
		return this->context_->getDevice()->createRenderState(desc);
	}

	GraphicsPipelinePtr
	ScriptableRenderContext::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
	{
		return this->context_->getDevice()->createRenderPipeline(desc);
	}

	GraphicsDescriptorSetPtr
	ScriptableRenderContext::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorSet(desc);
	}

	GraphicsDescriptorSetLayoutPtr
	ScriptableRenderContext::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
	{
		return this->context_->getDevice()->createDescriptorSetLayout(desc);
	}

	GraphicsDescriptorPoolPtr
	ScriptableRenderContext::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
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
	ScriptableRenderContext::setStencilCompareMask(StencilFaceFlags face, std::uint32_t mask) noexcept
	{
		this->context_->setStencilCompareMask(face, mask);
	}
	std::uint32_t
	ScriptableRenderContext::getStencilCompareMask(StencilFaceFlags face) noexcept
	{
		return this->context_->getStencilCompareMask(face);
	}

	void
	ScriptableRenderContext::setStencilReference(StencilFaceFlags face, std::uint32_t reference) noexcept
	{
		this->context_->setStencilReference(face, reference);
	}

	std::uint32_t
	ScriptableRenderContext::getStencilReference(StencilFaceFlags face) noexcept
	{
		return this->context_->getStencilReference(face);
	}

	void
	ScriptableRenderContext::setStencilWriteMask(StencilFaceFlags face, std::uint32_t mask) noexcept
	{
		this->context_->setStencilWriteMask(face, mask);
	}

	std::uint32_t
	ScriptableRenderContext::getStencilWriteMask(StencilFaceFlags face) noexcept
	{
		return this->context_->getStencilWriteMask(face);
	}

	void
	ScriptableRenderContext::setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept
	{
		this->context_->setRenderPipeline(pipeline);
	}

	GraphicsPipelinePtr
	ScriptableRenderContext::getRenderPipeline() const noexcept
	{
		return this->context_->getRenderPipeline();
	}

	void
	ScriptableRenderContext::setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept
	{
		this->context_->setDescriptorSet(descriptorSet);
	}

	GraphicsDescriptorSetPtr
	ScriptableRenderContext::getDescriptorSet() const noexcept
	{
		return this->context_->getDescriptorSet();
	}

	void
	ScriptableRenderContext::setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept
	{
		this->context_->setVertexBufferData(i, data, offset);
	}

	GraphicsDataPtr
	ScriptableRenderContext::getVertexBufferData(std::uint32_t i) const noexcept
	{
		return this->context_->getVertexBufferData(i);
	}

	void
	ScriptableRenderContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, IndexFormat indexType) noexcept
	{
		this->context_->setIndexBufferData(data, offset, indexType);
	}

	GraphicsDataPtr
	ScriptableRenderContext::getIndexBufferData() const noexcept
	{
		return this->context_->getIndexBufferData();
	}

	void
	ScriptableRenderContext::configureTarget(const GraphicsFramebufferPtr& target) noexcept
	{
		this->context_->setFramebuffer(target);
	}

	void
	ScriptableRenderContext::configureClear(ClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept
	{
		this->context_->clearFramebuffer(0, flags, color, depth, stencil);
	}

	void
	ScriptableRenderContext::discardFramebuffer(const GraphicsFramebufferPtr& src, ClearFlags flags) noexcept
	{
		this->context_->discardFramebuffer(src, flags);
	}

	void
	ScriptableRenderContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const math::float4& v1, const GraphicsFramebufferPtr& dest, const math::float4& v2, SamplerFilter filter) noexcept
	{
		this->context_->blitFramebuffer(src, v1, dest, v2, filter);
	}

	void
	ScriptableRenderContext::readFramebuffer(std::uint32_t i, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
	{
		this->context_->readFramebuffer(i, texture, miplevel, x, y, width, height);
	}

	void
	ScriptableRenderContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
	{
		this->context_->readFramebufferToCube(i, face, texture, miplevel, x, y, width, height);
	}

	GraphicsFramebufferPtr
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
	ScriptableRenderContext::drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
	{
		this->context_->drawIndirect(data, offset, drawCount, stride);
	}

	void
	ScriptableRenderContext::drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
	{
		this->context_->drawIndexedIndirect(data, offset, drawCount, stride);
	}

	void
	ScriptableRenderContext::compileMaterial(const std::shared_ptr<Material>& material, const RenderingData& renderingData)
	{
		auto it = this->materials_.find(material.get());
		if (it == this->materials_.end())
			this->materials_[material.get()] = std::make_shared<ScriptableRenderMaterial>(this->context_, material, renderingData);
	}

	void
	ScriptableRenderContext::setMaterial(const std::shared_ptr<Material>& material, const RenderingData& renderingData, const Camera& camera, const Geometry& geometry)
	{
		assert(material);

		auto it = this->materials_.find(material.get());
		if (it == this->materials_.end())
		{
			auto& pipeline = renderingData.materials_.at(material.get());
			pipeline->update(renderingData, camera, geometry);

			this->setRenderPipeline(pipeline->getPipeline());
			this->setDescriptorSet(pipeline->getDescriptorSet());
		}
		else
		{
			auto& pipeline = this->materials_.at(material.get());
			pipeline->update(renderingData, camera, geometry);

			this->setRenderPipeline(pipeline->getPipeline());
			this->setDescriptorSet(pipeline->getDescriptorSet());
		}

	}

	void
	ScriptableRenderContext::generateMipmap(const std::shared_ptr<GraphicsTexture>& texture) noexcept
	{
		this->context_->generateMipmap(texture);
	}

	void
	ScriptableRenderContext::drawMesh(const std::shared_ptr<Mesh>& mesh, std::size_t subset, const RenderingData& renderingData)
	{
		auto it = this->buffers_.find(mesh.get());
		if (it == this->buffers_.end())
		{
			auto& buffer = renderingData.buffers_.at(mesh.get());
			this->setVertexBufferData(0, buffer->getVertexBuffer(), 0);
			this->setIndexBufferData(buffer->getIndexBuffer(), 0, IndexFormat::UInt32);

			if (buffer->getIndexBuffer())
				this->drawIndexed((std::uint32_t)buffer->getNumIndices(subset), 1, (std::uint32_t)buffer->getStartIndices(subset), 0, 0);
			else
				this->draw((std::uint32_t)buffer->getNumVertices(), 1, 0, 0);
		}
		else
		{
			auto& buffer = buffers_.at(mesh.get());
			this->setVertexBufferData(0, buffer->getVertexBuffer(), 0);
			this->setIndexBufferData(buffer->getIndexBuffer(), 0, IndexFormat::UInt32);

			if (buffer->getIndexBuffer())
				this->drawIndexed((std::uint32_t)buffer->getNumIndices(subset), 1, (std::uint32_t)buffer->getStartIndices(subset), 0, 0);
			else
				this->draw((std::uint32_t)buffer->getNumVertices(), 1, 0, 0);
		}
	}

	void
	ScriptableRenderContext::drawRenderers(const Geometry& geometry, const Camera& camera, const RenderingData& renderingData, const std::shared_ptr<Material>& overrideMaterial) noexcept
	{
		if (camera.getLayer() != geometry.getLayer())
			return;

		if (geometry.getVisible())
		{
			auto numMaterials = geometry.getMaterials().size();

			for (std::size_t i = 0; i < numMaterials; i++)
			{
				auto mesh = geometry.getMesh();
				auto material = geometry.getMaterial(i);
				if (material && overrideMaterial)
				{
					if (material->getPrimitiveType() == overrideMaterial->getPrimitiveType())
						material = overrideMaterial;
				}

				if (material && mesh && i < mesh->getNumSubsets())
				{
					this->setMaterial(overrideMaterial ? overrideMaterial : material, renderingData, camera, geometry);
					this->drawMesh(mesh, i, renderingData);
				}
			}
		}
	}

	void
	ScriptableRenderContext::drawRenderers(const std::vector<Geometry*>& geometries, const Camera& camera, const RenderingData& renderingData, const std::shared_ptr<Material>& overrideMaterial) noexcept
	{
		for (auto& geometry : geometries)
			this->drawRenderers(*geometry, camera, renderingData, overrideMaterial);
	}
}
