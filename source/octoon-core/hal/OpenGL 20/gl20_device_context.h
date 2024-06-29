#ifndef OCTOON_GL20_DEVICE_CONTEXT_H_
#define OCTOON_GL20_DEVICE_CONTEXT_H_

#include "gl20_types.h"

namespace octoon
{
	namespace hal
	{
		class GL20DeviceContext final : public GraphicsContext
		{
			OctoonDeclareSubClass(GL20DeviceContext, GraphicsContext)
		public:
			GL20DeviceContext() noexcept;
			~GL20DeviceContext() noexcept;

			bool setup(const GraphicsContextDesc& desc) noexcept;
			void close() noexcept;

			void renderBegin() noexcept override;
			void renderEnd() noexcept override;

			void setViewport(std::uint32_t i, const math::float4& viewport) noexcept override;
			const math::float4& getViewport(std::uint32_t i) const noexcept override;

			void setScissor(std::uint32_t i, const math::uint4& scissor) noexcept override;
			const math::uint4& getScissor(std::uint32_t i) const noexcept override;

			void setStencilCompareMask(StencilFaceFlags face, std::uint32_t mask) noexcept override;
			std::uint32_t getStencilCompareMask(StencilFaceFlags face) noexcept override;

			void setStencilReference(StencilFaceFlags face, std::uint32_t reference) noexcept override;
			std::uint32_t getStencilReference(StencilFaceFlags face) noexcept override;

			void setStencilWriteMask(StencilFaceFlags face, std::uint32_t mask) noexcept override;
			std::uint32_t getStencilWriteMask(StencilFaceFlags face) noexcept override;

			void setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept override;
			GraphicsPipelinePtr getRenderPipeline() const noexcept override;

			void setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept override;
			GraphicsDescriptorSetPtr getDescriptorSet() const noexcept override;

			void setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept override;
			GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept override;

			void setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, IndexFormat indexType) noexcept override;
			GraphicsDataPtr getIndexBufferData() const noexcept override;

			void generateMipmap(const std::shared_ptr<GraphicsTexture>& texture) noexcept override;

			void setFramebuffer(const GraphicsFramebufferPtr& target) noexcept override;
			void clearFramebuffer(std::uint32_t i, ClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept override;
			void discardFramebuffer(const GraphicsFramebufferPtr& src, std::uint32_t i) noexcept override;
			void blitFramebuffer(const GraphicsFramebufferPtr& src, const math::float4& v1, const GraphicsFramebufferPtr& dest, const math::float4& v2, SamplerFilter filter) noexcept override;
			void readFramebuffer(std::uint32_t i, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept override;
			void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept override;
			GraphicsFramebufferPtr getFramebuffer() const noexcept override;

			void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept override;
			void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept override;
			void drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept override;
			void drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept override;

			void present() noexcept override;

			void startDebugControl() noexcept;
			void stopDebugControl() noexcept;

		private:
			bool initStateSystem() noexcept;

		private:
			friend class GL20Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL20DeviceContext(const GL20DeviceContext&) noexcept = delete;
			GL20DeviceContext& operator=(const GL20DeviceContext&) noexcept = delete;

		private:
			GLfloat _clearDepth;
			GLint   _clearStencil;
			math::float4 _clearColor;

			math::float4 _viewport;
			math::uint4 _scissor;

			GLenum  _indexType;
			GLintptr _indexOffset;

			GL20PipelinePtr _pipeline;
			GL20SwapchainPtr _glcontext;
			GL20DescriptorSetPtr _descriptorSet;
			GL20FramebufferPtr _framebuffer;
			GL20VertexBuffers _vertexBuffers;
			GL20GraphicsDataPtr _indexBuffer;
			GL20ProgramPtr _program;
			GL20GraphicsStatePtr _state;
			GL20GraphicsStatePtr _stateDefault;
			RenderStateDesc _stateCaptured;

			bool _needUpdatePipeline;
			bool _needUpdateDescriptor;
			bool _needUpdateVertexBuffers;

			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif