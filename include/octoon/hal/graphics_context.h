#ifndef OCTOON_GRAPHICS_CONTEXT_H_
#define OCTOON_GRAPHICS_CONTEXT_H_

#include <octoon/hal/graphics_child.h>

namespace octoon
{
	namespace hal
	{
		class OCTOON_EXPORT GraphicsContextDesc final
		{
		public:
			GraphicsContextDesc() noexcept;
			GraphicsContextDesc(GraphicsSwapchainPtr swapchain) noexcept;
			~GraphicsContextDesc() noexcept;

			void setSwapchain(const GraphicsSwapchainPtr& swapchain) noexcept;
			GraphicsSwapchainPtr getSwapchain() const noexcept;

		private:
			GraphicsSwapchainPtr _swapchain;
		};

		class OCTOON_EXPORT GraphicsContext : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsContext, GraphicsChild)
		public:
			GraphicsContext() noexcept = default;
			virtual ~GraphicsContext() = default;

			virtual void renderBegin() noexcept = 0;
			virtual void renderEnd() noexcept = 0;

			virtual void setViewport(std::uint32_t i, const float4& viewport) noexcept = 0;
			virtual const float4& getViewport(std::uint32_t i) const noexcept = 0;

			virtual void setScissor(std::uint32_t i, const uint4& scissor) noexcept = 0;
			virtual const uint4& getScissor(std::uint32_t i) const noexcept = 0;

			virtual void setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept = 0;
			virtual std::uint32_t getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept = 0;

			virtual void setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept = 0;
			virtual std::uint32_t getStencilReference(GraphicsStencilFaceFlags face) noexcept = 0;

			virtual void setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept = 0;
			virtual std::uint32_t getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept = 0;

			virtual void setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept = 0;
			virtual GraphicsPipelinePtr getRenderPipeline() const noexcept = 0;

			virtual void setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept = 0;
			virtual GraphicsDescriptorSetPtr getDescriptorSet() const noexcept = 0;

			virtual void setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept = 0;
			virtual GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept = 0;

			virtual void setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept = 0;
			virtual GraphicsDataPtr getIndexBufferData() const noexcept = 0;

			virtual void generateMipmap(const GraphicsTexturePtr& texture) noexcept = 0;

			virtual void setFramebuffer(const GraphicsFramebufferPtr& target) noexcept = 0;
			virtual void clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept = 0;
			virtual void discardFramebuffer(const GraphicsFramebufferPtr& src, GraphicsClearFlags flags = GraphicsClearFlagBits::AllBit) noexcept = 0;
			virtual void blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept = 0;
			virtual void readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept = 0;
			virtual void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept = 0;
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
}

#endif