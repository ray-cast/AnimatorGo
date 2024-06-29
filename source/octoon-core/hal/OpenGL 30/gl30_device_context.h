#ifndef OCTOON_GL30_DEVICE_CONTEXT_H_
#define OCTOON_GL30_DEVICE_CONTEXT_H_

#include "gl30_types.h"

namespace octoon
{
	namespace hal
	{
		class GL30DeviceContext final : public GraphicsContext
		{
			OctoonDeclareSubClass(GL30DeviceContext, GraphicsContext)
		public:
			GL30DeviceContext() noexcept;
			~GL30DeviceContext() noexcept;

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
			void discardFramebuffer(const GraphicsFramebufferPtr& src, ClearFlags flags) noexcept override;
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
			friend class GL30Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL30DeviceContext(const GL30DeviceContext&) noexcept = delete;
			GL30DeviceContext& operator=(const GL30DeviceContext&) noexcept = delete;

		private:

			GLfloat _clearDepth;
			GLint   _clearStencil;
			math::float4 _clearColor;

			std::vector<math::float4> _viewports;
			std::vector<math::uint4> _scissors;
			std::vector<GLenum> _attachments;

			GLuint _inputLayout;

			GLenum  _indexType;
			GLintptr _indexOffset;

			GL30SwapchainPtr _glcontext;

			GL30PipelinePtr _pipeline;
			GL30DescriptorSetPtr _descriptorSet;
			GL30FramebufferPtr _framebuffer;
			GL30FramebufferPtr _readFramebuffer;
			GL30FramebufferPtr _drawFramebuffer;
			GL30VertexBuffers _vertexBuffers;
			GL30GraphicsDataPtr _indexBuffer;
			GL30ProgramPtr _program;
			GL30GraphicsStatePtr _state;
			GL30GraphicsStatePtr _stateDefault;
			RenderStateDesc _stateCaptured;

			bool _needUpdatePipeline;
			bool _needUpdateDescriptor;
			bool _needUpdateVertexBuffers;

			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif