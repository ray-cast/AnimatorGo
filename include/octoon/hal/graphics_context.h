#ifndef OCTOON_GRAPHICS_CONTEXT_H_
#define OCTOON_GRAPHICS_CONTEXT_H_

#include <octoon/hal/graphics_texture.h>
#include <octoon/hal/graphics_swapchain.h>

namespace octoon
{
	class OCTOON_EXPORT GraphicsContextDesc final
	{
	public:
		GraphicsContextDesc() noexcept;
		GraphicsContextDesc(std::shared_ptr<GraphicsSwapchain> swapchain) noexcept;
		~GraphicsContextDesc() noexcept;

		void setSwapchain(const std::shared_ptr<GraphicsSwapchain>& swapchain) noexcept;
		std::shared_ptr<GraphicsSwapchain> getSwapchain() const noexcept;

	private:
		std::shared_ptr<GraphicsSwapchain> _swapchain;
	};

	class OCTOON_EXPORT GraphicsContext : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsContext, GraphicsResource)
	public:
		GraphicsContext() noexcept = default;
		virtual ~GraphicsContext() = default;

		virtual void renderBegin() noexcept = 0;
		virtual void renderEnd() noexcept = 0;

		virtual void setViewport(std::uint32_t i, const math::float4& viewport) noexcept = 0;
		virtual const math::float4& getViewport(std::uint32_t i) const noexcept = 0;

		virtual void setScissor(std::uint32_t i, const math::uint4& scissor) noexcept = 0;
		virtual const math::uint4& getScissor(std::uint32_t i) const noexcept = 0;

		virtual void setStencilCompareMask(StencilFaceFlags face, std::uint32_t mask) noexcept = 0;
		virtual std::uint32_t getStencilCompareMask(StencilFaceFlags face) noexcept = 0;

		virtual void setStencilReference(StencilFaceFlags face, std::uint32_t reference) noexcept = 0;
		virtual std::uint32_t getStencilReference(StencilFaceFlags face) noexcept = 0;

		virtual void setStencilWriteMask(StencilFaceFlags face, std::uint32_t mask) noexcept = 0;
		virtual std::uint32_t getStencilWriteMask(StencilFaceFlags face) noexcept = 0;

		virtual void setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept = 0;
		virtual GraphicsPipelinePtr getRenderPipeline() const noexcept = 0;

		virtual void setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept = 0;
		virtual GraphicsDescriptorSetPtr getDescriptorSet() const noexcept = 0;

		virtual void setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept = 0;
		virtual GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept = 0;

		virtual void setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, IndexFormat indexType) noexcept = 0;
		virtual GraphicsDataPtr getIndexBufferData() const noexcept = 0;

		virtual void generateMipmap(const std::shared_ptr<GraphicsTexture>& texture) noexcept = 0;

		virtual void setFramebuffer(const GraphicsFramebufferPtr& target) noexcept = 0;
		virtual void clearFramebuffer(std::uint32_t i, ClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept = 0;
		virtual void discardFramebuffer(const GraphicsFramebufferPtr& src, ClearFlags flags = ClearFlagBits::AllBit) noexcept = 0;
		virtual void blitFramebuffer(const GraphicsFramebufferPtr& src, const math::float4& v1, const GraphicsFramebufferPtr& dest, const math::float4& v2, SamplerFilter filter) noexcept = 0;
		virtual void readFramebuffer(std::uint32_t i, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept = 0;
		virtual void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept = 0;
		virtual GraphicsFramebufferPtr getFramebuffer() const noexcept = 0;

		virtual void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept = 0;
		virtual void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept = 0;
		virtual void drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept = 0;
		virtual void drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept = 0;

		virtual void present() noexcept = 0;

	private:
		GraphicsContext(const GraphicsContext&) noexcept = delete;
		GraphicsContext& operator=(const GraphicsContext&) noexcept = delete;
	};
}

#endif