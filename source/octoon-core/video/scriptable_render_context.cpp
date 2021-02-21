#include <octoon/video/scriptable_render_context.h>
#include <octoon/hal/graphics_context.h>
#include <octoon/hal/graphics_device.h>

namespace octoon::video
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
	ScriptableRenderContext::setFramebuffer(const hal::GraphicsFramebufferPtr& target) noexcept
	{
		this->context_->setFramebuffer(target);
	}

	void
	ScriptableRenderContext::clearFramebuffer(std::uint32_t i, hal::GraphicsClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept
	{
		this->context_->clearFramebuffer(i, flags, color, depth, stencil);
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
	ScriptableRenderContext::generateMipmap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		this->context_->generateMipmap(texture);
	}

}
